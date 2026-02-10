#include <gst/gst.h>
#include <stdio.h>

/* Structure to contain all our information */
typedef struct _CustomData {
    GstElement *playbin;
    GMainLoop *loop;
    gboolean playing;
    gboolean terminate;
    gboolean seek_enabled;
    gboolean seek_done;
    gint64 duration;
} CustomData;

/* Handler for messages on the bus */
static gboolean handle_message(GstBus *bus, GstMessage *msg, CustomData *data) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_EOS:
            g_print("\nEnd-Of-Stream reached.\n");
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_DURATION_CHANGED:
            /* The duration has changed, mark the current one as invalid */
            data->duration = GST_CLOCK_TIME_NONE;
            break;
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state, pending_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin)) {
                g_print("Pipeline state changed from %s to %s:\n",
                    gst_element_state_get_name(old_state),
                    gst_element_state_get_name(new_state));
                
                data->playing = (new_state == GST_STATE_PLAYING);

                if (data->playing) {
                    GstQuery *query;
                    gint64 start, end;
                    query = gst_query_new_seeking(GST_FORMAT_TIME);
                    if (gst_element_query(data->playbin, query)) {
                        gst_query_parse_seeking(query, NULL, &data->seek_enabled, &start, &end);
                        if (data->seek_enabled) {
                            g_print("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                                GST_TIME_ARGS(start), GST_TIME_ARGS(end));
                        } else {
                            g_print("Seeking is DISABLED for this stream.\n");
                        }
                    } else {
                        g_printerr("Seeking query failed.");
                    }
                    gst_query_unref(query);
                }
            }
        } break;
        default:
            break;
    }

    /* We want to keep receiving messages */
    return TRUE;
}

/* This function is called periodically to refresh the GUI */
static gboolean refresh_ui(CustomData *data) {
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 current = -1;

    /* We do not want to update anything unless we are in the PLAYING state */
    if (!data->playing)
        return TRUE;

    /* If we didn't know it yet, query the stream duration */
    if (!GST_CLOCK_TIME_IS_VALID(data->duration)) {
        if (!gst_element_query_duration(data->playbin, fmt, &data->duration)) {
            g_printerr("Could not query current duration.\n");
        }
    }

    /* Query the current position of the stream */
    if (gst_element_query_position(data->playbin, fmt, &current)) {
        /* Print current position and total duration */
        g_print("\rPosition %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT,
            GST_TIME_ARGS(current), GST_TIME_ARGS(data->duration));
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    CustomData data;
    GstBus *bus;
    GstStateChangeReturn ret;

    if (argc != 2) {
        g_printerr("Usage: %s <media file URI or path>\n", argv[0]);
        g_printerr("Example: %s file:///path/to/video.mp4\n", argv[0]);
        g_printerr("     or: %s /path/to/video.mp4\n", argv[0]);
        return -1;
    }

    /* Initialize custom data structure */
    memset(&data, 0, sizeof(data));
    data.duration = GST_CLOCK_TIME_NONE;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the playbin element */
    data.playbin = gst_element_factory_make("playbin", "playbin");

    if (!data.playbin) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Set the URI to play */
    gchar *uri;
    if (gst_uri_is_valid(argv[1])) {
        uri = g_strdup(argv[1]);
    } else {
        uri = gst_filename_to_uri(argv[1], NULL);
    }
    g_object_set(data.playbin, "uri", uri, NULL);
    g_print("Now playing: %s\n", uri);
    g_free(uri);

    /* Connect to interesting signals in playbin */
    bus = gst_element_get_bus(data.playbin);
    gst_bus_add_watch(bus, (GstBusFunc)handle_message, &data);

    /* Start playing */
    ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(data.playbin);
        return -1;
    }

    /* Create a GLib Main Loop and set it to run */
    data.loop = g_main_loop_new(NULL, FALSE);

    /* Register a function that will periodically check for position changes */
    g_timeout_add_seconds(1, (GSourceFunc)refresh_ui, &data);

    /* Listen to the bus */
    g_main_loop_run(data.loop);

    /* Free resources */
    g_main_loop_unref(data.loop);
    gst_object_unref(bus);
    gst_element_set_state(data.playbin, GST_STATE_NULL);
    gst_object_unref(data.playbin);

    g_print("\nPlayback finished.\n");
    return 0;
}
