#include "pipeline.h"

namespace vgraph {
namespace video {
namespace consts{
    const std::string& h264_profile("high");
    const int output_bitrate(80*1024);
}
namespace elem {
    namespace name {
        const std::string& pipeline("pipeline");

        // input
        const std::string& source("source");
        const std::string& decode("decode"); //decode does demuxing too

        // audio
        const std::string& audio_convert("audio_convert");
        const std::string& audio_resample("audio_resample");
        const std::string& audio_encode("audio_encode");

        // video
        const std::string& video_convert("video_convert");
        const std::string& video_glupload("video_glupload");
        const std::string& video_rotate("video_rotate");
        const std::string& video_scale("video_scale");
        const std::string& video_overlay("video_overlay");
        const std::string& video_compositor("video_compositor");
        const std::string& video_convert2("video_convert2");
        const std::string& video_encode("video_encode");
        const std::string& video_h264_parse("video_h264_parse");

        // output
        const std::string& mux("mux");
        const std::string& sink("sink");
    }

    std::map<std::string, std::string> type = {
        {name::source,           "filesrc"},
        {name::decode,           "decodebin"},
        {name::audio_convert,    "audioconvert"},
        {name::audio_resample,   "audioresample"},
        {name::audio_encode,     "avenc_aac"},
        {name::video_convert,    "videoconvert"},
        {name::video_glupload,   "glupload"},
        {name::video_rotate,     "glvideoflip"},// {name::video_rotate,     "videoflip"},
        {name::video_scale,      "gltransformation"},//{name::video_scale,      "videoscale"},
        // {name::video_overlay,    "cairooverlay"}, // FIXME switch (somehow) to gloverlay compositor (theoretically should be able to use cairo still)
        {name::video_overlay,    "overlaycomposition"},
        {name::video_compositor, "gloverlaycompositor"},
        // {name::video_convert2,   "videoconvert"},
        {name::video_encode,     "nvh264enc"},// {name::video_encode,     "x264enc"},
        {name::video_h264_parse, "h264parse"},
        {name::mux,              "mp4mux"},
        {name::sink,             "filesink"}
    };
}

namespace helper {
    void pad_added_cb(GstElement* src, GstPad* new_pad, pipeline* ptr)
    {
        ptr->pad_added_handler(src, new_pad);
    }

    // basing a lot on: https://gitlab.freedesktop.org/gstreamer/gstreamer-rs/-/blob/main/examples/src/bin/overlay-composition.rs
    // and on cairooverlay impl: https://github.com/GStreamer/gst-plugins-good/blob/master/ext/cairo/gstcairooverlay.c
    GstVideoOverlayComposition* draw_cb(GstElement* overlay, GstSample* sample, overlay::overlay* ptr)
    {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstClockTime timestamp = GST_BUFFER_PTS(buffer);

        long raw_stamp = GST_TIME_AS_USECONDS(timestamp);
        static long first_raw_stamp = raw_stamp;
        double stamp = (raw_stamp - first_raw_stamp) / 1000000.0;

        //FIXME it can be much better and cleaner and probably faster - i.e. probably no need to go through surface->cr->surface->sample
        // to be figured out later
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 3840, 2160);

        cairo_t* cr = cairo_create(surface);
        ptr->draw(cr, stamp);
        cairo_destroy(cr);

        // int stride = cairo_image_surface_get_stride(surface); //whetever that is
        unsigned char* data = cairo_image_surface_get_data(surface);
        int size = cairo_image_surface_get_height (surface) * cairo_image_surface_get_stride (surface);
        
        GstBuffer* out_buffer = gst_buffer_new_wrapped_full(
            (GstMemoryFlags)0, cairo_image_surface_get_data(surface), size, 0, size, surface, (GDestroyNotify)cairo_surface_destroy);

        GstVideoOverlayCompositionMeta *composition_meta = gst_buffer_get_video_overlay_composition_meta(buffer);

        gsize offset[GST_VIDEO_MAX_PLANES] = { 0, };
        gint stride[GST_VIDEO_MAX_PLANES] = { cairo_image_surface_get_stride(surface), };
        gst_buffer_add_video_meta_full(
            out_buffer,
            GST_VIDEO_FRAME_FLAG_NONE,
            (G_BYTE_ORDER == G_LITTLE_ENDIAN ? GST_VIDEO_FORMAT_BGRA : GST_VIDEO_FORMAT_ARGB),
            3840, 2160, 1, offset, stride);

        GstVideoOverlayRectangle * rect = gst_video_overlay_rectangle_new_raw(
            out_buffer, 0, 0, 3840, 2160, GST_VIDEO_OVERLAY_FORMAT_FLAG_PREMULTIPLIED_ALPHA);//GST_VIDEO_OVERLAY_FORMAT_FLAG_NONE);

        GstVideoOverlayComposition* comp = gst_video_overlay_composition_new(rect);

        gst_video_overlay_rectangle_unref(rect);
        gst_buffer_unref(out_buffer);

        return comp;
    }

    // void draw_cb(GstElement* overlay, cairo_t* cr, guint64 timestamp, guint64 duration, overlay::overlay* ptr)
    // {
    //     long raw_stamp = GST_TIME_AS_USECONDS(timestamp);
    //     static long first_raw_stamp = raw_stamp;
    //     double stamp = (raw_stamp - first_raw_stamp) / 1000000.0;

    //     ptr->draw(cr, stamp);
    // }
}


pipeline::pipeline(const std::string& input_path, const std::string& output_path, overlay::overlay& overlay):
    input_path_(input_path),
    output_path_(output_path),
    overlay_(overlay)
{}

pipeline::~pipeline()
{
    log.info("Tearing down pipeline");

    if (elements_.contains(elem::name::pipeline) && elements_.at(elem::name::pipeline) != nullptr) {
        GstElement* pipeline_ = elements_.at(elem::name::pipeline);
        gst_element_set_state(pipeline_, GST_STATE_NULL);
        gst_object_unref(pipeline_);
    }
    elements_.clear();
}

bool pipeline::build()
{
    log.info("Creating pipeline elements");

    log.debug("Creating pipeline bin");
    GstElement* pipeline_ptr = gst_pipeline_new(elem::name::pipeline.c_str());

    bool ok = true;
    if (nullptr != pipeline_ptr) {
        log.debug("Created pipeline bin");
        elements_[elem::name::pipeline] = pipeline_ptr;
    } else {
        log.error("Creating pipeline bin failed");
        ok = false;
    }

    GstBin* bin = GST_BIN(pipeline_ptr);

    for (auto const &[name,type] : elem::type) {
        log.debug("Creating {} pipeline element ({})", type, name);
        GstElement* ptr = gst_element_factory_make(type.c_str(), name.c_str());

        if (nullptr != ptr) {
            log.debug("Created {} pipeline element ({})", type, name);
            elements_[name] = ptr;
            gst_bin_add(bin, ptr);
        } else {
            log.error("Creating {} pipeline element ({}) failed", type, name);
            ok = false;
        }
    }

    return ok;
}

bool pipeline::link()
{
    log.info("Linking pipeline elements");
    bool ok = true;

    //link input: source -> decode
    log.debug("Linking input path");
    ok = link_elements(elem::name::source, elem::name::decode) && ok;

    //link audio: audio_convert -> audio_resample -> audio_encode -> mux
    log.debug("Linking audio path");
    ok = link_elements(elem::name::audio_convert, elem::name::audio_resample) && ok;
    ok = link_elements(elem::name::audio_resample, elem::name::audio_encode) && ok;
    ok = link_elements(elem::name::audio_encode, elem::name::mux) && ok;

    //link video: video_convert -> video_rotate -> video_overlay -> video_convert2 -> video_encode -> mux
    log.debug("Linking video path");
    // ok = link_elements(elem::name::video_convert, elem::name::video_rotate) && ok;
    // ok = link_elements(elem::name::video_rotate, elem::name::video_overlay) && ok;
    // ok = link_elements(elem::name::video_overlay, elem::name::video_convert2) && ok;

    /* REMOVE FROM THIS LINE */
    GstCaps* filter_raw = gst_caps_from_string("video/x-raw");
    ok = link_elements(elem::name::video_convert, elem::name::video_glupload, filter_raw) && ok;

    GstCaps* filter_gpu = gst_caps_from_string("video/x-raw(memory:GLMemory)");
    ok = link_elements(elem::name::video_glupload, elem::name::video_rotate, filter_gpu) && ok;
    ok = link_elements(elem::name::video_rotate, elem::name::video_scale, filter_gpu) && ok;

    GstCaps* filter_scale = gst_caps_from_string("video/x-raw(memory:GLMemory),width=3840,height=2160");

    // GstCaps* filter_scale = gst_caps_new_simple("video/x-raw(memory:GLMemory)",
    //                                             "width", G_TYPE_INT, 3840,
    //                                             "height", G_TYPE_INT, 2160,
    //                                             nullptr);

    ok = link_elements(elem::name::video_scale, elem::name::video_overlay, filter_scale) && ok;

    GstCaps* filter_composition = gst_caps_from_string("video/x-raw(memory:GLMemory, meta:GstVideoOverlayComposition)");
    ok = link_elements(elem::name::video_overlay, elem::name::video_compositor, filter_composition) && ok;
    ok = link_elements(elem::name::video_compositor, elem::name::video_encode, filter_gpu) && ok;

    
    /* REMOVE TO THIS LINE */
    
    // ok = link_elements(elem::name::video_convert2, elem::name::video_encode) && ok;

    GstCaps* filter = gst_caps_new_simple("video/x-h264",
                                          "profile", G_TYPE_STRING, consts::h264_profile.c_str(),
                                          nullptr);

    ok = link_elements(elem::name::video_encode, elem::name::video_h264_parse, filter) && ok;
    ok = link_elements(elem::name::video_h264_parse, elem::name::mux) && ok;
    gst_caps_unref(filter);
    filter = nullptr;

    //link output: mux -> sink
    log.debug("Linking output path");
    ok = link_elements(elem::name::mux, elem::name::sink) && ok;

    //NOTE: decode->audio_convert and decode->video_convert are not linked here - will be linked dynamically
    return ok;
}

bool pipeline::init()
{
    log.info("Initializing pipeline");

    log.debug("Pipeline elements_ basic config");
    g_object_set(elements_[elem::name::source], "location", input_path_.c_str(), nullptr);
    g_object_set(elements_[elem::name::sink], "location", output_path_.c_str(), nullptr);
    g_object_set(elements_[elem::name::video_rotate], "video-direction", GST_VIDEO_ORIENTATION_AUTO, nullptr);
    g_object_set(elements_[elem::name::video_encode], "bitrate", consts::output_bitrate, nullptr);

    log.debug("Connect callback to 'pad-added' signal of '{}' element", elem::name::decode);
    g_signal_connect(elements_[elem::name::decode], "pad-added", G_CALLBACK(helper::pad_added_cb), this);
    g_signal_connect (elements_[elem::name::video_overlay], "draw", G_CALLBACK (helper::draw_cb), const_cast<overlay::overlay*>(&overlay_));

    return true;
}

bool pipeline::run()
{
    log.info("Starting the pipeline execution");

    log.debug("Setting pipeline state to PLAYING");
    GstStateChangeReturn ret = gst_element_set_state(elements_[elem::name::pipeline], GST_STATE_PLAYING);

    if (GST_STATE_CHANGE_FAILURE == ret) {
        log.error("Pipeline state change has failed (target state: PLAYING)");
        return false;
    }

    // listen to the bus
    GstBus *bus = gst_element_get_bus(elements_[elem::name::pipeline]);
    GstMessage *msg;

    bool terminate = false;
    bool failed = false;

    do {
        msg = gst_bus_timed_pop_filtered(bus, 1000 * GST_MSECOND,
            static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        if (msg != NULL) {
            GError* err;
            gchar* debug_info;

            switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error(msg, &err, &debug_info);
                    log.error("Error received from {}: {}", GST_OBJECT_NAME(msg->src), err->message);
                    log.error("Debugging info: {}", debug_info ? debug_info : "none");
                    g_clear_error(&err);
                    g_free(debug_info);
                    terminate = true;
                    failed = true;
                    break;
                case GST_MESSAGE_EOS:
                    log.info("Pipeline finished - End-Of-Stream reached");
                    terminate = true;
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    // handle only if msg comes from pipeline
                    if (GST_MESSAGE_SRC(msg) == GST_OBJECT(elements_[elem::name::pipeline])) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                        log.info("Pipeline state changed from {} to {}",
                            gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
                    }
                    break;
                default:
                    //shouldn't reach as we didn't register for any other message type
                    log.warning("Unexpected message received from backend");
                    break;
            }
            gst_message_unref(msg);
        } else {
            //no message received during timeout - update time display
            GstElement* ptr = elements_[elem::name::pipeline];

            gint64 pos, len;
            if (gst_element_query_position(ptr, GST_FORMAT_TIME, &pos) && gst_element_query_duration(ptr, GST_FORMAT_TIME, &len)) {
                double pct = 100.0 * static_cast<double>(pos) / static_cast<double>(len);
                // g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT " (%.1f%%)\r", GST_TIME_ARGS(pos), GST_TIME_ARGS(len), pct);
                g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT " (%.1f%%)\n", GST_TIME_ARGS(pos), GST_TIME_ARGS(len), pct);
            }
        }
    } while (!terminate);

    gst_object_unref(bus);
    return !failed;
}

bool pipeline::link_elements(const std::string& src, const std::string& dest, GstCaps* filter)
{
    log.debug("Linking {} to {}", src, dest);

    bool ok = false;
    if (nullptr == filter) {
        ok = gst_element_link(elements_[src], elements_[dest]);
    } else {
        ok = gst_element_link_filtered(elements_[src], elements_[dest], filter);
    }

    if (!ok)
        log.error("Failed to link {} to {}", src, dest);
    return ok;
}

void pipeline::pad_added_handler(GstElement* src, GstPad* new_pad)
{
    log.debug("Received new pad '{}' from '{}'", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    GstCaps* new_pad_caps = gst_pad_get_current_caps(new_pad);
    GstStructure* new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    std::string new_pad_type = gst_structure_get_name(new_pad_struct);

    log.debug("New pad type is '{}'", new_pad_type);

    GstPad* target_pad = nullptr;

    if (new_pad_type.starts_with("video/x-raw")) {
        log.debug("Target pad is located in {} element", elem::name::video_convert);
        target_pad = gst_element_get_static_pad(elements_[elem::name::video_convert], "sink");
    } else if (new_pad_type.starts_with("audio/x-raw")) {
        log.debug("Target pad is located in {} element", elem::name::audio_convert);
        target_pad = gst_element_get_static_pad(elements_[elem::name::audio_convert], "sink");
    } else {
        log.debug("Unsupported pad type - ignoring");
    }

    if (nullptr != target_pad) {
        if (!gst_pad_is_linked(target_pad)) {
            GstPadLinkReturn ret = gst_pad_link(new_pad, target_pad);
            if (GST_PAD_LINK_FAILED(ret)) {
                log.error("Linking pad '{}' has failed", new_pad_type);
            } else {
                log.debug("Link succeeded (type '{}')", new_pad_type);
            }
        } else {
            log.debug("Target pad is already linked - ignoring received '{}' pad", new_pad_type);
        }

        gst_object_unref(target_pad);
    }

    if (nullptr != new_pad_caps)
        gst_caps_unref(new_pad_caps);
}

} // namespace video
} // namespace vgraph
