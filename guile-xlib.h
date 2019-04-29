#ifndef GUILE2_XLIB_XLIB_H
#define GUILE2_XLIB_XLIB_H

#include <X11/Xlib.h>
#include <libguile.h>

/* Note on differences between this interface and raw Xlib in C.

   Some differences are inevitable.  When the power of Xlib is made
   available in an general purpose interpreted environment like Guile,
   we need to make sure that the interface cannot be called in a way
   that would cause the environment as a whole to crash or hang.  For
   example, in C you can call XCloseDisplay and then, say,
   XDestroyWindow for the display that you have just closed; and your
   program will probably hang or crash as a result.  An interface like
   the one implemented here must protect the environment against such
   problems by detecting and rejecting invalid usage.

   In practice, this means that the interface needs to track the
   current state of X resources like displays and windows.  So the
   Guile Xlib interface differs from C Xlib at least in that it stores
   certain additional state information and uses this information to
   detect and disallow invalid usage.

   Given that some difference is inevitable, one piece of Schemely
   behaviour is sufficiently useful - and sufficiently easier to
   implement in this interface than in a Scheme layer above it - that
   it merits a further small departure from C Xlib.  This is the
   automatic freeing of X resources when the interface-level objects
   associated with them are garbage collected.  It applies to displays
   (using XCloseDisplay), windows (XDestroyWindow) and non-default gcs
   (XFreeGC).  Note that it is still possible to free these resources
   explicitly, using the x-close-display!, x-destroy-window! and
   x-free-gc! primitives respectively.

   Many further differences (between this interface and C Xlib) are
   possible, but none are compelling.  The X people presumably thought
   quite carefully about the structure and completeness of the Xlib
   interface, and that's worth benefitting from.  Layers presenting a
   graphical X interface with a different structure, or from a
   different angle, can easily be implemented in Scheme on top of
   this one. */


/* Note on garbage collection and freeing of X resources.

   The one wrinkle in implementing automatic freeing of X resources is
   that almost all X resources depend on a valid display, so we have
   to be careful that the display resource is always freed (using
   XCloseDisplay) last of all.

   In most cases this is handled by having resource smobs include a
   reference to the display smob.  But there is still a problem when
   all remaining X resource references are dropped between one GC
   cycle and the next: when this happens, the next GC sweep could free
   the display smob before it gets to some of the other resource
   smobs.

   Fortunately, resource smobs can check, in their free functions,
   whether this has happened, by looking at the SCM_TYP16 of their
   reference to the display smob.  If the display smob is still valid,
   this will be scm_tc16_xdisplay, and the relevant X resource should
   be freed as normal.  If the display smob has been freed earlier in
   this sweep, GC will have set its SCM_TYP16 to scm_tc_free_cell;
   this indicates that XCloseDisplay has already been called, and so
   the relevant X resource no longer needs to be freed. */


/* SMOB TYPES */

typedef struct xdisplay_t
{
  /* The underlying Xlib display pointer. */
  Display *dsp;

  /* State - open/closed. */
  int state;

#define XDISPLAY_STATE_OPEN         1
#define XDISPLAY_STATE_CLOSED       2
#define XDISPLAY_STATE_ANY          ( XDISPLAY_STATE_OPEN | XDISPLAY_STATE_CLOSED )

  /* Cached default gc smob for this display. */
  SCM gc;

} xdisplay_t;

typedef struct xscreen_t
{
  /* The display that this screen is on. */
  SCM dsp;

  /* The underlying Xlib screen structure. */
  Screen *scr;

} xscreen_t;

typedef struct xwindow_t
{
  /* The display that this window is on. */
  SCM dsp;

  /* The underlying Xlib window ID. */
  Window win;

  /* State - mapped/unmapped/destroyed. */
  int state;

#define XWINDOW_STATE_UNMAPPED      1
#define XWINDOW_STATE_MAPPED        2
#define XWINDOW_STATE_DESTROYED     4
#define XWINDOW_STATE_THIRD_PARTY   8
#define XWINDOW_STATE_PIXMAP        16

} xwindow_t;

typedef struct xgc_t
{
  /* The display that this GC belongs to. */
  SCM dsp;

  /* The underlying Xlib GC ID. */
  GC gc;

  /* State - default/created/freed. */
  int state;

#define XGC_STATE_DEFAULT           1
#define XGC_STATE_CREATED           2
#define XGC_STATE_FREED             4

} xgc_t;


/* DECLARATIONS */

int scm_tc16_xdisplay = 0;
int scm_tc16_xscreen = 0;
int scm_tc16_xwindow = 0;
int scm_tc16_xgc = 0;

SCM resource_id_hash;

#define XDISPLAY(display) ((xdisplay_t *) SCM_SMOB_DATA (display))
#define XSCREEN(screen)   ((xscreen_t *) SCM_SMOB_DATA (screen))

#define XDATA_ARCS            0
#define XDATA_LINES           1
#define XDATA_POINTS          2
#define XDATA_SEGMENTS        3
#define XDATA_RECTANGLES      4

SCM scm_x_open_display_x (SCM host);
SCM scm_x_close_display_x (SCM display);
SCM scm_x_no_op_x (SCM display);
SCM scm_x_flush_x (SCM display);
SCM scm_x_connection_number (SCM display);
SCM scm_x_screen_count (SCM display);
SCM scm_x_default_screen (SCM display);
SCM scm_x_q_length (SCM display);
SCM scm_x_server_vendor (SCM display);
SCM scm_x_protocol_version (SCM display);
SCM scm_x_protocol_revision (SCM display);
SCM scm_x_vendor_release (SCM display);
SCM scm_x_display_string (SCM display);
SCM scm_x_bitmap_unit (SCM display);
SCM scm_x_bitmap_bit_order (SCM display);
SCM scm_x_bitmap_pad (SCM display);
SCM scm_x_image_byte_order (SCM display);
SCM scm_x_next_request (SCM display);
SCM scm_x_last_known_request_processed (SCM display);
SCM scm_x_display_of (SCM whatever);
SCM scm_x_all_planes (void);
SCM scm_x_root_window (SCM display, SCM screen);
SCM scm_x_black_pixel (SCM display, SCM screen);
SCM scm_x_white_pixel (SCM display, SCM screen);
SCM scm_x_display_width (SCM display, SCM screen);
SCM scm_x_display_height (SCM display, SCM screen);
SCM scm_x_display_width_mm (SCM display, SCM screen);
SCM scm_x_display_height_mm (SCM display, SCM screen);
SCM scm_x_display_planes (SCM display, SCM screen);
SCM scm_x_display_cells (SCM display, SCM screen);
SCM scm_x_screen_of_display (SCM display, SCM screen);
SCM scm_x_screen_number_of_screen (SCM screen);
SCM scm_x_min_colormaps (SCM display, SCM screen);
SCM scm_x_max_colormaps (SCM display, SCM screen);

xwindow_t * valid_win (SCM arg, int pos, int expected, const char *func);

SCM scm_x_create_window_x (SCM display);          /* @@@ simplified */
SCM scm_x_map_window_x (SCM window);
SCM scm_x_unmap_window_x (SCM window);
SCM scm_x_destroy_window_x (SCM window);
SCM scm_x_clear_window_x (SCM window);
SCM scm_x_clear_area_x (SCM window, SCM x, SCM y, SCM width, SCM height, SCM exposures);

SCM scm_x_create_pixmap_x (SCM display, SCM screen, SCM width, SCM height, SCM depth);
SCM scm_x_copy_area_x (SCM source, SCM destination, SCM gc, SCM src_x, SCM src_y, SCM width, SCM height, SCM dst_x, SCM dst_y);

SCM scm_x_default_gc (SCM display, SCM screen);
SCM scm_x_free_gc_x (SCM gc);
SCM scm_x_create_gc_x (SCM gc, SCM changes);
SCM scm_x_change_gc_x (SCM gc, SCM changes);
SCM scm_x_set_dashes_x (SCM gc, SCM offset, SCM dashes);
SCM scm_x_set_clip_rectangles_x (SCM gc, SCM x, SCM y, SCM rectangles, SCM ordering);
SCM scm_x_copy_gc_x (SCM src, SCM dst, SCM fields);

SCM scm_x_draw_arcs_x (SCM window, SCM gc, SCM arcs);
SCM scm_x_draw_lines_x (SCM window, SCM gc, SCM points);
SCM scm_x_draw_points_x (SCM window, SCM gc, SCM points);
SCM scm_x_draw_segments_x (SCM window, SCM gc, SCM segments);
SCM scm_x_draw_rectangles_x (SCM window, SCM gc, SCM rectangles);

SCM scm_x_check_mask_event_x (SCM display, SCM mask, SCM event);
SCM scm_x_check_typed_event_x (SCM display, SCM type, SCM event);
SCM scm_x_check_typed_window_event_x (SCM window, SCM type, SCM event);
SCM scm_x_check_window_event_x (SCM window, SCM mask, SCM event);
SCM scm_x_events_queued_x (SCM display, SCM mode);
SCM scm_x_pending_x (SCM display);
SCM scm_x_mask_event_x (SCM display, SCM mask, SCM event);
SCM scm_x_next_event_x (SCM display, SCM event);
SCM scm_x_peek_event_x (SCM display, SCM event);
SCM scm_x_select_input_x (SCM window, SCM mask);
SCM scm_x_window_event_x (SCM window, SCM mask, SCM event);

void init_xlib_core (void);

#endif /* GUILE2_XLIB_XLIB_H */
