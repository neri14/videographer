/* compile with:
 * g++ -o pipeline pipeline.cpp -std=c++23 `pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0` */

#include <iostream>
#include <format>
#include <string>

extern "C" {
    #include <gst/gst.h>
    #include <gst/video/video.h>
}

//gst data struct
struct CustomData {
    GstElement* pipeline;

    // input
    GstElement* source;
    GstElement* decode;

    // audio
    GstElement* audioconvert;
    GstElement* resample;
    GstElement* audioenc;

    // video
    GstElement* videoconvert;
    GstElement* rotate;
    GstElement* text;
    GstElement* scale;
    GstElement* encode;

    // output
    GstElement* mux;
    GstElement* sink;

};

void pad_added_handler(GstElement* src, GstPad* pad, CustomData* data);

gboolean print_position (GstElement *pipeline)
{
  gint64 pos, len;

  if (gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos)
    && gst_element_query_duration (pipeline, GST_FORMAT_TIME, &len)) {
    g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
         GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
  }

  /* call me again */
  return TRUE;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << std::format("Usage: {} INPUT OUTPUT", argv[0]) << std::endl;
        return 1;
    }

    std::string input_file(argv[1]);
    std::string output_file(argv[2]);

    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    bool terminate;

    // init gstreamer - not passing args to handle them myself
    gst_init(nullptr, nullptr);


    // create pipeline elements
    data.source = gst_element_factory_make ("filesrc", "source");

    data.decode = gst_element_factory_make ("decodebin", "decode");

    data.audioconvert = gst_element_factory_make ("audioconvert", "audioconvert");
    data.resample = gst_element_factory_make ("audioresample", "resample");
    data.audioenc = gst_element_factory_make("avenc_aac", "audioenc");

    data.videoconvert = gst_element_factory_make ("videoconvert", "videoconvert");
    data.rotate = gst_element_factory_make ("videoflip", "rotate");
    data.text = gst_element_factory_make ("textoverlay", "text");
    data.encode = gst_element_factory_make ("x264enc", "encode");

    data.mux = gst_element_factory_make ("mp4mux", "mux");
    data.sink = gst_element_factory_make ("filesink", "sink");

    // create empty pipeline
    data.pipeline = gst_pipeline_new("test-pipeline");
    if (!data.pipeline || !data.source || !data.decode ||
        !data.audioconvert || !data.resample || !data.audioenc ||
        !data.videoconvert || !data.rotate || !data.text || !data.encode ||
        !data.mux || !data.sink) {
        std::cerr << "Not all elements could be created" << std::endl;
        return 1;
    }

    // build pipeline - note the source is not linked here
    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.decode, nullptr);
    gst_bin_add_many(GST_BIN(data.pipeline), data.audioconvert, data.resample, data.audioenc, nullptr);
    gst_bin_add_many(GST_BIN(data.pipeline), data.videoconvert, data.rotate, data.text, data.encode, nullptr);
    gst_bin_add_many(GST_BIN(data.pipeline), data.mux, data.sink, nullptr);

    if (!gst_element_link_many(data.source, data.decode, nullptr)) {
        std::cerr << "Source elements could not be linked" << std::endl;
        gst_object_unref(data.pipeline);
        return 1;
    }
    if (!gst_element_link_many(data.audioconvert, data.resample, data.audioenc, data.mux, nullptr)) {
        std::cerr << "Audio elements could not be linked" << std::endl;
        gst_object_unref(data.pipeline);
        return 1;
    }
    if (!gst_element_link_many(data.videoconvert, data.rotate, data.text, data.encode, nullptr)) {//data.scale, nullptr)) {
        std::cerr << "Video elements could not be linked" << std::endl;
        gst_object_unref(data.pipeline);
        return 1;
    }

    {
    // specifically setting up filter on link to force use profile high in x264enc
        GstCaps *caps = gst_caps_new_simple("video/x-h264",
                                            "profile", G_TYPE_STRING, "high",
                                            NULL);
        if (!gst_element_link_filtered(data.encode, data.mux, caps)) {
            std::cerr << "Encode-MUX elements could not be linked" << std::endl;
            gst_object_unref(data.pipeline);
            return 1;
        }
    }

    if (!gst_element_link_many(data.mux, data.sink, nullptr)) {
        std::cerr << "Output elements could not be linked" << std::endl;
        gst_object_unref(data.pipeline);
        return 1;
    }


    // set source file
    g_object_set(data.source, "location", input_file.c_str(), NULL);

    // set static text
    g_object_set(data.text, "text", "This is a test text overlay", NULL);
    g_object_set(data.text, "font-desc", "Monospace, 72", NULL);

    // set bitrate
    g_object_set(data.encode, "bitrate", 15*1024, NULL);

    // set output file
    g_object_set(data.sink, "location", output_file.c_str(), NULL);

    // Connect callback to pad-added signal
    g_signal_connect(data.decode, "pad-added", G_CALLBACK(pad_added_handler), &data);
    //here note how we can pass additional data to a callback

    //start playing
    ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
    if (GST_STATE_CHANGE_FAILURE == ret) {
        std::cerr << "Unable to set the pipeline to playing state" << std::endl;
        gst_object_unref(data.pipeline);
        return 1;
    }

    GstVideoOrientationMethod rotation = GST_VIDEO_ORIENTATION_IDENTITY;

    // listen to the bus
    bus = gst_element_get_bus(data.pipeline);
    do {
        msg = gst_bus_timed_pop_filtered(bus, 1000 * GST_MSECOND,
            static_cast<GstMessageType>(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_TAG));
        
        // parse message
        if (msg != NULL) {
            GError* err;
            gchar* debug_info;

            switch (GST_MESSAGE_TYPE (msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error(msg, &err, &debug_info);
                    std::cerr << std::format("Error received from {}: {}", GST_OBJECT_NAME(msg->src), err->message) << std::endl;
                    std::cerr << std::format("Debugging info: {}", debug_info ? debug_info : "none") << std::endl;
                    g_clear_error(&err);
                    g_free(debug_info);
                    terminate = true;
                    break;
                case GST_MESSAGE_EOS:
                    std::cout << "End-Of-Stream reached" << std::endl;
                    terminate = true;
                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    // handle only if msg comes from pipeline
                    if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data.pipeline)) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                        std::cout << std::format("Pipeline state changed from {} to {}",
                            gst_element_state_get_name(old_state), gst_element_state_get_name(new_state)) << std::endl;
                    }
                    break;
                case GST_MESSAGE_TAG:
                    GstTagList* tags;
                    gst_message_parse_tag(msg, &tags);

                    {
                        GstVideoOrientationMethod new_rotation = GST_VIDEO_ORIENTATION_IDENTITY;
                        if(gst_video_orientation_from_tag(tags, &new_rotation)) {
                            if (new_rotation != rotation) {
                                rotation = new_rotation;
                                g_object_set(data.rotate, "video-direction", rotation, NULL);
                                std::cout << "rotation: " << static_cast<int>(rotation) << std::endl;
                                //FIXME one of elements is stripping tags - autorotation doesn't work anymore
                            }
                        }
                    }
                    gst_tag_list_unref(tags);
                    break;
                default:
                    //shouldn't reach as we didn't register for any other message type
                    std::cerr << "Unexpected message received" << std::endl;
                    break;
            }
            gst_message_unref(msg);
        } else {
            //no message - timeout so print time
            print_position(data.pipeline);
        }
    } while (!terminate);

    // free resources
    gst_object_unref(bus);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);

    return 0;
}

// callback for pad-added signal on data.source
void pad_added_handler(GstElement* src, GstPad* new_pad, CustomData* data)
{
    GstPad* audio_sink_pad = gst_element_get_static_pad(data->audioconvert, "sink");
    GstPad* video_sink_pad = gst_element_get_static_pad(data->videoconvert, "sink");
    GstPadLinkReturn ret;
    GstCaps* new_pad_caps = nullptr;
    GstStructure* new_pad_struct = nullptr;
    const gchar* new_pad_type = nullptr;

    std::cout << std::format("Received new pad '{}' from '{}'",
        GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src)) << std::endl;

    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);

    if (std::string(new_pad_type).starts_with("video/x-raw")) {
        if (!gst_pad_is_linked(video_sink_pad)) {
            ret = gst_pad_link(new_pad, video_sink_pad);
            if (GST_PAD_LINK_FAILED(ret)) {
                std::cerr << std::format("New pad type is '{}' but link failed", new_pad_type) << std::endl;
            } else {
                std::cout << std::format("Link succeeded (type '{}')", new_pad_type) << std::endl;
            }
        } else {
            //if video converter is already linked - nothing to do
            std::cout << std::format("Video is already linked, ignoring received '{}' pad", new_pad_type) << std::endl;
        }
    } else if (std::string(new_pad_type).starts_with("audio/x-raw")) {
        if (!gst_pad_is_linked(audio_sink_pad)) {
            ret = gst_pad_link(new_pad, audio_sink_pad);
            if (GST_PAD_LINK_FAILED(ret)) {
                std::cerr << std::format("New pad type is '{}' but link failed", new_pad_type) << std::endl;
            } else {
                std::cout << std::format("Link succeeded (type '{}')", new_pad_type) << std::endl;
            }
        } else {
            //if audio converter is already linked - nothing to do
            std::cout << std::format("Audio is already linked, ignoring received '{}' pad", new_pad_type) << std::endl;
        }
    } else {
        std::cout << std::format("New pad has type '{}' which is not supported - ignoring", new_pad_type) << std::endl;
    }

    if (new_pad_caps != nullptr)
        gst_caps_unref(new_pad_caps);
    gst_object_unref(audio_sink_pad);
    gst_object_unref(video_sink_pad);
}
