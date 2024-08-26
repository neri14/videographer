#include "generator.h"

namespace vgraph {
namespace video {

namespace elem {
    namespace name {
        const std::string pipeline("pipeline");

        const std::string source("source");
        const std::string decode("decode"); //decode does demuxing too

        const std::string audio_convert("audio_convert");
        const std::string audio_resample("audio_resample");
        const std::string audio_encode("audio_encode");
    
        const std::string video_convert("video_convert");
        const std::string video_overlay("video_overlay");
        const std::string video_h264_parse("video_h264_parse");

        const std::string video_rotate("video_rotate");
        const std::string video_scale("video_scale");
        const std::string video_convert2("video_convert2");
        const std::string video_encode("video_encode");

        const std::string video_glupload("video_glupload");
        const std::string video_rotate_gpu("video_rotate_gpu");
        const std::string video_scale_gpu("video_scale_gpu");
        const std::string video_compositor_gpu("video_compositor_gpu");
        const std::string video_encode_gpu("video_encode_gpu");
    
        const std::string mux("mux");
        const std::string sink("sink");
    }
    namespace type {
        const std::string filesrc("filesrc");
        const std::string decodebin("decodebin");

        const std::string audioconvert("audioconvert");
        const std::string audioresample("audioresample");
        const std::string avenc_aac("avenc_aac");

        const std::string videoconvert("videoconvert");
        const std::string overlaycomposition("overlaycomposition");
        const std::string h264parse("h264parse");

        const std::string videoflip("videoflip");
        const std::string videoscale("videoscale");
        const std::string x264enc("x264enc");
    
        const std::string glupload("glupload");
        const std::string glvideoflip("glvideoflip");
        const std::string gltransformation("gltransformation");
        const std::string gloverlaycompositor("gloverlaycompositor");
        const std::string nvh264enc("nvh264enc");
    
        const std::string mp4mux("mp4mux");
        const std::string filesink("filesink");
    }
}

namespace helper {
    void pad_added_cb(GstElement* src, GstPad* new_pad, generator* ptr)
    {
        ptr->pad_added_handler(src, new_pad);
    }

    /**NOTE:
      * should only be called from draw_cb due to static first_raw_stamp
      * hack for offsetting received timestamps into correct time domain
      */
    double normalize_timestamp(GstClockTime timestamp)
    {
        long raw_stamp = GST_TIME_AS_USECONDS(timestamp);
        static long first_raw_stamp = raw_stamp;
        return (raw_stamp - first_raw_stamp) / 1000000.0;
    }

    GstVideoOverlayComposition* draw_cb(GstElement* overlay, GstSample* sample, overlay::overlay* ptr)
    {
        // create surface for drawing
        cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ptr->width, ptr->height);

        // draw overlay onto surface
        cairo_t* cr = cairo_create(surface);
        ptr->draw(cr, normalize_timestamp(GST_BUFFER_PTS(gst_sample_get_buffer(sample))));
        cairo_destroy(cr);

        // make outgoing buffer out of surface
        int size = cairo_image_surface_get_height(surface) * cairo_image_surface_get_stride(surface);
        GstBuffer* out_buffer = gst_buffer_new_wrapped_full(
            (GstMemoryFlags)0, cairo_image_surface_get_data(surface), size, 0, size, surface, (GDestroyNotify)cairo_surface_destroy);
            //note - normally we'd call cairo_surface_destroy to avoid memory leaks,
            // but here we pass it as "notify" callback so that destroying buffer destroys surface too

        // add metadata to outgoing buffer
        gsize offset[GST_VIDEO_MAX_PLANES] = { 0, };
        gint stride[GST_VIDEO_MAX_PLANES] = { cairo_image_surface_get_stride(surface), };
        gst_buffer_add_video_meta_full(
            out_buffer,
            GST_VIDEO_FRAME_FLAG_NONE,
            (G_BYTE_ORDER == G_LITTLE_ENDIAN ? GST_VIDEO_FORMAT_BGRA : GST_VIDEO_FORMAT_ARGB),
            ptr->width, ptr->height, 1/*number of planes*/, offset, stride);

        //create overlay rectangle from outgoing buffer
        GstVideoOverlayRectangle * rect = gst_video_overlay_rectangle_new_raw(
            out_buffer, 0/*x*/, 0/*y*/, ptr->width, ptr->height, GST_VIDEO_OVERLAY_FORMAT_FLAG_PREMULTIPLIED_ALPHA);
        gst_buffer_unref(out_buffer);

        //create overlay composition from rectangle
        GstVideoOverlayComposition* comp = gst_video_overlay_composition_new(rect);
        gst_video_overlay_rectangle_unref(rect);

        return comp;
    }
}

bool operator==(const pipeline_element& lhs, const pipeline_element& rhs)
{
    return lhs.name == rhs.name;
}


generator::generator(const std::string& input_path,
                   const std::string& output_path,
                   overlay::overlay& overlay,
                   bool gpu,
                   std::pair<int,int> output_resolution,
                   int output_bitrate):
    gpu_(gpu),
    input_path_(input_path),
    output_path_(output_path),
    overlay_(overlay),
    output_resolution_(output_resolution),
    output_bitrate_(output_bitrate)
{}

generator::~generator()
{
    log.info("Tearing down pipeline");

    if (elements_.contains(elem::name::pipeline) && elements_.at(elem::name::pipeline) != nullptr) {
        GstElement* pipeline_ = elements_.at(elem::name::pipeline);
        gst_element_set_state(pipeline_, GST_STATE_NULL);
        gst_object_unref(pipeline_);
    }
    elements_.clear();
}

bool generator::generate()
{
    log.info("Video generation starting");
    gst_init(nullptr, nullptr);

    bool ok = true;
    ok = ok && setup();
    ok = ok && build();
    ok = ok && link();
    ok = ok && config_elements();
    ok = ok && connect_signals();
    ok = ok && execute();

    if (ok) {
        log.info("Video generation finished");
    } else {
        log.error("Video generation failed");
    }

    return ok;
}

bool generator::setup()
{
    bool ok = true;
    ok = setup_input() && ok;
    ok = setup_audio() && ok;
    if (gpu_) {
        ok = setup_video_gpu() && ok;
    } else {
        ok = setup_video_cpu() && ok;
    }
    ok = setup_output() && ok;

    return true;
}

bool generator::setup_input()
{
    input_path = {
        {elem::name::source, elem::type::filesrc},
        {elem::name::decode, elem::type::decodebin}
    };
    return true;
}

bool generator::setup_audio()
{
    audio_path = {
        {elem::name::audio_convert,  elem::type::audioconvert},
        {elem::name::audio_resample, elem::type::audioresample},
        {elem::name::audio_encode,   elem::type::avenc_aac}
    };
    return true;
}

bool generator::setup_video_gpu()
{
    video_path = {
        {elem::name::video_convert,        elem::type::videoconvert,        "video/x-raw"},
        {elem::name::video_glupload,       elem::type::glupload,            "video/x-raw(memory:GLMemory)"},
        {elem::name::video_rotate_gpu,     elem::type::glvideoflip,         "video/x-raw(memory:GLMemory)"},
        {elem::name::video_scale_gpu,      elem::type::gltransformation,    std::format("video/x-raw(memory:GLMemory),width={},height={}",
                                                                                        output_resolution_.first,
                                                                                        output_resolution_.second)},
        {elem::name::video_overlay,        elem::type::overlaycomposition,  "video/x-raw(memory:GLMemory, meta:GstVideoOverlayComposition)"},
        {elem::name::video_compositor_gpu, elem::type::gloverlaycompositor, "video/x-raw(memory:GLMemory)"},
        {elem::name::video_encode_gpu,     elem::type::nvh264enc,           "video/x-h264,profile=high"},
        {elem::name::video_h264_parse,     elem::type::h264parse}
    };
    return true;
}

bool generator::setup_video_cpu()
{
    video_path = {
        {elem::name::video_convert,    elem::type::videoconvert,        "video/x-raw"},
        {elem::name::video_rotate,     elem::type::videoflip},
        {elem::name::video_scale,      elem::type::videoscale,          std::format("video/x-raw,width={},height={}",
                                                                                    output_resolution_.first,
                                                                                    output_resolution_.second)},
        {elem::name::video_overlay,    elem::type::overlaycomposition},
        {elem::name::video_convert2,   elem::type::videoconvert},
        {elem::name::video_encode,     elem::type::x264enc,             "video/x-h264,profile=high"},
        {elem::name::video_h264_parse, elem::type::h264parse}
    };
    return true;
}

bool generator::setup_output()
{
    output_path = {
        {elem::name::mux,  elem::type::mp4mux},
        {elem::name::sink, elem::type::filesink}
    };
    return true;
}

bool generator::build()
{
    log.info("Creating pipeline elements");

    bool ok = true;
    
    ok = ok && build_pipeline_bin();
    ok = ok && build_pipeline_elements(input_path);
    ok = ok && build_pipeline_elements(audio_path);
    ok = ok && build_pipeline_elements(video_path);
    ok = ok && build_pipeline_elements(output_path);

    if (!ok) {
        log.error("Pipeline creation failed");
    }
    return ok;
}

bool generator::build_pipeline_bin()
{
    log.debug("Creating pipeline bin");
    GstElement* pipeline_ptr = gst_pipeline_new(elem::name::pipeline.c_str());

    bool ok = true;
    if (nullptr == pipeline_ptr) {
        log.error("Creating pipeline bin failed");
        return false;
    }

    log.debug("Created pipeline bin");
    elements_[elem::name::pipeline] = pipeline_ptr;
    return true;
}

bool generator::build_pipeline_elements(const std::vector<pipeline_element>& elems)
{
    GstBin* bin = GST_BIN(elements_[elem::name::pipeline]);

    bool ok = true;
    for (const pipeline_element& el : elems) {
        log.debug("Creating {} pipeline element ({})", el.type, el.name);
        GstElement* ptr = gst_element_factory_make(el.type.c_str(), el.name.c_str());

        if (nullptr != ptr) {
            log.debug("Created {} pipeline element ({})", el.type, el.name);
            elements_[el.name] = ptr;
            gst_bin_add(bin, ptr);
        } else {
            log.error("Creating {} pipeline element ({}) failed", el.type, el.name);
            ok = false;
        }
    }
    return ok;
}

bool generator::link()
{
    bool ok = true;

    ok = link_path_elements(input_path) && ok;
    ok = link_path_elements(audio_path) && ok;
    ok = link_path_elements(video_path) && ok;
    ok = link_path_elements(output_path) && ok;

    //link input->audio and input->video is dynamic
    ok = link_paths(audio_path, output_path) && ok;
    ok = link_paths(video_path, output_path) && ok;

    if (!ok) {
        log.error("Pipeline elements static linking failed");
    }
    return ok;
}

bool generator::link_path_elements(const std::vector<pipeline_element>& path)
{
    bool ok = true;

    auto last_it = path.begin();
    for (auto it = path.begin()+1; it != path.end(); it++) {
        ok = link_elements(*last_it, *it) && ok;
        last_it = it;
    }

    return ok;
}

bool generator::link_paths(const std::vector<pipeline_element>& from, const std::vector<pipeline_element>& to)
{
    return link_elements(from.back(), to.front());
}

bool generator::link_elements(const pipeline_element& src, const pipeline_element& dest)
{
    log.debug("Linking {} ({}) to {} ({}) with filter: {}",
              src.name, src.type, dest.name, dest.type,
              src.src_pad_filter ? *src.src_pad_filter : "none");

    bool ok = false;
    if (src.src_pad_filter) {
        GstCaps* filter = gst_caps_from_string(src.src_pad_filter->c_str());
        ok = gst_element_link_filtered(elements_[src.name], elements_[dest.name], filter);
        gst_caps_unref(filter);
    } else {
        ok = gst_element_link(elements_[src.name], elements_[dest.name]);
    }

    if (!ok)
        log.error("Failed to link {} ({}) to {} ({}) with filter: {}",
                  src.name, src.type, dest.name, dest.type,
                  src.src_pad_filter ? *src.src_pad_filter : "none");
    return ok;
}

bool generator::config_elements()
{//FIXME try to incorporate congig params into path vectors and auto configure them (note: different types)
    log.debug("Pipeline elements_ basic config");

    bool ok = true;
    if (elements_.contains(elem::name::source)) {
        g_object_set(elements_[elem::name::source], "location", input_path_.c_str(), nullptr);
    } else {
        log.error("Missing source element - unable to configure input path");
        ok = false;
    }

    if (elements_.contains(elem::name::sink)) {
        g_object_set(elements_[elem::name::sink], "location", output_path_.c_str(), nullptr);
    } else {
        log.error("Missing sink element - unable to configure output path");
        ok = false;
    }

    if (elements_.contains(elem::name::video_rotate)) {
        g_object_set(elements_[elem::name::video_rotate], "video-direction", GST_VIDEO_ORIENTATION_AUTO, nullptr);
    } else if (elements_.contains(elem::name::video_rotate_gpu)) {
        g_object_set(elements_[elem::name::video_rotate_gpu], "video-direction", GST_VIDEO_ORIENTATION_AUTO, nullptr);
    } else {
        log.error("Missing rotation capable element - unable to configure auto rotation");
        ok = false;
    }

    if (elements_.contains(elem::name::video_encode)) {
        g_object_set(elements_[elem::name::video_encode], "bitrate", output_bitrate_, nullptr);
    } else if (elements_.contains(elem::name::video_encode_gpu)) {
        g_object_set(elements_[elem::name::video_encode_gpu], "bitrate", output_bitrate_, nullptr);
    } else {
        log.error("Missing encoder element - unable to configure bitrate");
        ok = false;
    }

    return ok;
}

bool generator::connect_signals()
{
    log.debug("Connect callback to 'pad-added' signal of '{}' element", elem::name::decode);

    bool ok = true;

    if (elements_.contains(elem::name::decode)) {
        g_signal_connect(elements_[elem::name::decode], "pad-added", G_CALLBACK(helper::pad_added_cb), this);
    } else {
        log.error("Missing decoder element - unable to connect to 'pad-added' signal");
        ok = false;
    }

    if (elements_.contains(elem::name::video_overlay)) {
        g_signal_connect (elements_[elem::name::video_overlay], "draw", G_CALLBACK (helper::draw_cb), const_cast<overlay::overlay*>(&overlay_));
    } else {
        log.error("Missing overlay element - unable to connect to 'draw' signal");
        ok = false;
    }

    return ok;
}

bool generator::execute()
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

    if (failed) {
        log.error("Pipeline execution failed");
    }
    return !failed;
}

void generator::pad_added_handler(GstElement* src, GstPad* new_pad)
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
