/*****************************************************************************
 * keystone.c : Keystone / corner pin video filter for VLC
 *****************************************************************************
 * Copyright (C) 2026 VLC authors and VideoLAN
 *
 * Authors: VLC authors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>

#include <vlc_common.h>
#include <vlc_atomic.h>
#include <vlc_plugin.h>
#include <vlc_filter.h>
#include <vlc_mouse.h>
#include <vlc_picture.h>
#include "filter_picture.h"

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int  Create    ( vlc_object_t * );
static void Destroy   ( vlc_object_t * );

static picture_t *Filter( filter_t *, picture_t * );
static int Mouse( filter_t *, vlc_mouse_t *,
                  const vlc_mouse_t *, const vlc_mouse_t * );
static int KeystoneCallback( vlc_object_t *, char const *,
                             vlc_value_t, vlc_value_t, void * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/

#define FILTER_PREFIX "keystone-"

#define TL_X_TEXT N_("Top-left X offset")
#define TL_X_LONGTEXT N_( \
    "Horizontal offset for the top-left corner, as a fraction of the " \
    "image width (-1.0 to 1.0). Default: 0.0" )
#define TL_Y_TEXT N_("Top-left Y offset")
#define TL_Y_LONGTEXT N_( \
    "Vertical offset for the top-left corner, as a fraction of the " \
    "image height (-1.0 to 1.0). Default: 0.0" )
#define TR_X_TEXT N_("Top-right X offset")
#define TR_X_LONGTEXT N_( \
    "Horizontal offset for the top-right corner, as a fraction of the " \
    "image width (-1.0 to 1.0). Default: 0.0" )
#define TR_Y_TEXT N_("Top-right Y offset")
#define TR_Y_LONGTEXT N_( \
    "Vertical offset for the top-right corner, as a fraction of the " \
    "image height (-1.0 to 1.0). Default: 0.0" )
#define BL_X_TEXT N_("Bottom-left X offset")
#define BL_X_LONGTEXT N_( \
    "Horizontal offset for the bottom-left corner, as a fraction of the " \
    "image width (-1.0 to 1.0). Default: 0.0" )
#define BL_Y_TEXT N_("Bottom-left Y offset")
#define BL_Y_LONGTEXT N_( \
    "Vertical offset for the bottom-left corner, as a fraction of the " \
    "image height (-1.0 to 1.0). Default: 0.0" )
#define BR_X_TEXT N_("Bottom-right X offset")
#define BR_X_LONGTEXT N_( \
    "Horizontal offset for the bottom-right corner, as a fraction of the " \
    "image width (-1.0 to 1.0). Default: 0.0" )
#define BR_Y_TEXT N_("Bottom-right Y offset")
#define BR_Y_LONGTEXT N_( \
    "Vertical offset for the bottom-right corner, as a fraction of the " \
    "image height (-1.0 to 1.0). Default: 0.0" )
#define HANDLES_TEXT N_("Show corner handles")
#define HANDLES_LONGTEXT N_( \
    "Display draggable corner handles on the video. " \
    "Default: enabled" )

vlc_module_begin ()
    set_description( N_("Keystone / corner pin video filter") )
    set_shortname( N_("Keystone") )
    set_category( CAT_VIDEO )
    set_subcategory( SUBCAT_VIDEO_VFILTER )
    set_capability( "video filter", 0 )

    add_float_with_range( FILTER_PREFIX "tl-x", 0.0, -1.0, 1.0,
                          TL_X_TEXT, TL_X_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "tl-y", 0.0, -1.0, 1.0,
                          TL_Y_TEXT, TL_Y_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "tr-x", 0.0, -1.0, 1.0,
                          TR_X_TEXT, TR_X_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "tr-y", 0.0, -1.0, 1.0,
                          TR_Y_TEXT, TR_Y_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "bl-x", 0.0, -1.0, 1.0,
                          BL_X_TEXT, BL_X_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "bl-y", 0.0, -1.0, 1.0,
                          BL_Y_TEXT, BL_Y_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "br-x", 0.0, -1.0, 1.0,
                          BR_X_TEXT, BR_X_LONGTEXT, false )
        change_safe()
    add_float_with_range( FILTER_PREFIX "br-y", 0.0, -1.0, 1.0,
                          BR_Y_TEXT, BR_Y_LONGTEXT, false )
        change_safe()

    add_bool( FILTER_PREFIX "show-handles", true,
              HANDLES_TEXT, HANDLES_LONGTEXT, false )
        change_safe()

    add_shortcut( "keystone" )
    set_callbacks( Create, Destroy )
vlc_module_end ()

static const char *const ppsz_filter_options[] = {
    "tl-x", "tl-y", "tr-x", "tr-y",
    "bl-x", "bl-y", "br-x", "br-y",
    "show-handles",
    NULL
};

/* Names of the 8 corner offset variables, for iteration */
static const char *const ppsz_corner_vars[] = {
    FILTER_PREFIX "tl-x", FILTER_PREFIX "tl-y",
    FILTER_PREFIX "tr-x", FILTER_PREFIX "tr-y",
    FILTER_PREFIX "bl-x", FILTER_PREFIX "bl-y",
    FILTER_PREFIX "br-x", FILTER_PREFIX "br-y",
};

/*****************************************************************************
 * Constants
 *****************************************************************************/
#define HANDLE_SIZE  12   /* Half-size of corner handle in pixels */
#define HANDLE_Y    255   /* Handle color: white */
#define HANDLE_U    128
#define HANDLE_V    128
#define ACTIVE_Y     76   /* Active/dragged handle color: red */
#define ACTIVE_U     85
#define ACTIVE_V    255
#define HOVER_Y     191   /* Hover handle color: orange */
#define HOVER_U      36
#define HOVER_V     222

/*****************************************************************************
 * filter_sys_t
 *****************************************************************************/
struct filter_sys_t
{
    /* Corner offsets (normalized -1..1) */
    vlc_atomic_float f_tl_x, f_tl_y;
    vlc_atomic_float f_tr_x, f_tr_y;
    vlc_atomic_float f_bl_x, f_bl_y;
    vlc_atomic_float f_br_x, f_br_y;

    /* Mouse interaction state */
    atomic_int  i_drag_corner;  /* -1 = none, 0=TL, 1=TR, 2=BL, 3=BR */
    atomic_int  i_hover_corner; /* -1 = none, corner closest to mouse */
    atomic_bool b_show_handles; /* Whether to draw corner handles */
};

/*****************************************************************************
 * ComputeHomography: solve 8x8 linear system for perspective transform
 *****************************************************************************
 * Computes the 3x3 homography matrix H mapping destination corners to
 * source corners (inverse mapping for rendering).
 *
 * H maps (dx,dy) -> (sx,sy) via:
 *   sx = (h[0]*dx + h[1]*dy + h[2]) / (h[6]*dx + h[7]*dy + 1)
 *   sy = (h[3]*dx + h[4]*dy + h[5]) / (h[6]*dx + h[7]*dy + 1)
 *
 * Returns false if the system is degenerate.
 *****************************************************************************/
static bool ComputeHomography( double h[8],
    double sx0, double sy0, double dx0, double dy0,
    double sx1, double sy1, double dx1, double dy1,
    double sx2, double sy2, double dx2, double dy2,
    double sx3, double sy3, double dx3, double dy3 )
{
    /*
     * Each point correspondence (dx,dy) -> (sx,sy) gives 2 equations:
     *   h0*dx + h1*dy + h2 - h6*dx*sx - h7*dy*sx = sx
     *   h3*dx + h4*dy + h5 - h6*dx*sy - h7*dy*sy = sy
     */
    double A[8][9];
    const double *dx_arr[] = { &dx0, &dx1, &dx2, &dx3 };
    const double *dy_arr[] = { &dy0, &dy1, &dy2, &dy3 };
    const double *sx_arr[] = { &sx0, &sx1, &sx2, &sx3 };
    const double *sy_arr[] = { &sy0, &sy1, &sy2, &sy3 };

    for( int i = 0; i < 4; i++ )
    {
        double dx = *dx_arr[i], dy = *dy_arr[i];
        double sx = *sx_arr[i], sy = *sy_arr[i];

        A[2*i][0] = dx;  A[2*i][1] = dy;  A[2*i][2] = 1.0;
        A[2*i][3] = 0.0; A[2*i][4] = 0.0; A[2*i][5] = 0.0;
        A[2*i][6] = -dx * sx; A[2*i][7] = -dy * sx;
        A[2*i][8] = sx;

        A[2*i+1][0] = 0.0; A[2*i+1][1] = 0.0; A[2*i+1][2] = 0.0;
        A[2*i+1][3] = dx;  A[2*i+1][4] = dy;  A[2*i+1][5] = 1.0;
        A[2*i+1][6] = -dx * sy; A[2*i+1][7] = -dy * sy;
        A[2*i+1][8] = sy;
    }

    /* Gauss-Jordan elimination with partial pivoting */
    for( int col = 0; col < 8; col++ )
    {
        int pivot = col;
        double max_val = fabs( A[col][col] );
        for( int row = col + 1; row < 8; row++ )
        {
            double v = fabs( A[row][col] );
            if( v > max_val )
            {
                max_val = v;
                pivot = row;
            }
        }

        if( max_val < 1e-12 )
            return false;

        if( pivot != col )
        {
            for( int j = 0; j < 9; j++ )
            {
                double tmp = A[col][j];
                A[col][j] = A[pivot][j];
                A[pivot][j] = tmp;
            }
        }

        double div = A[col][col];
        for( int j = col; j < 9; j++ )
            A[col][j] /= div;

        for( int row = 0; row < 8; row++ )
        {
            if( row == col )
                continue;
            double factor = A[row][col];
            for( int j = col; j < 9; j++ )
                A[row][j] -= factor * A[col][j];
        }
    }

    for( int i = 0; i < 8; i++ )
        h[i] = A[i][8];

    return true;
}

/*****************************************************************************
 * RenderPlane: apply perspective transform to one picture plane
 *****************************************************************************/
static void RenderPlane( const plane_t *p_src, plane_t *p_dst,
                         int i_y_width, int i_y_height,
                         const double h[8], int i_plane )
{
    const int i_dst_width  = p_dst->i_visible_pitch / p_dst->i_pixel_pitch;
    const int i_dst_height = p_dst->i_visible_lines;
    const int i_src_width  = p_src->i_visible_pitch / p_src->i_pixel_pitch;
    const int i_src_height = p_src->i_visible_lines;

    const double f_scale_x = (double)i_y_width / i_dst_width;
    const double f_scale_y = (double)i_y_height / i_dst_height;
    const double f_inv_scale_x = (double)i_dst_width / i_y_width;
    const double f_inv_scale_y = (double)i_dst_height / i_y_height;

    const uint8_t fill = ( i_plane == U_PLANE || i_plane == V_PLANE )
                         ? 0x80 : 0x00;

    const double h0_sx = h[0] * f_scale_x;
    const double h3_sx = h[3] * f_scale_x;
    const double h6_sx = h[6] * f_scale_x;

    for( int y = 0; y < i_dst_height; y++ )
    {
        uint8_t *p_out = &p_dst->p_pixels[y * p_dst->i_pitch];

        const double dy = y * f_scale_y;

        double num_x = h[1] * dy + h[2];
        double num_y = h[4] * dy + h[5];
        double den   = h[7] * dy + 1.0;

        for( int x = 0; x < i_dst_width; x++ )
        {
            if( fabs( den ) < 1e-12 )
            {
                p_out[x] = fill;
                num_x += h0_sx;
                num_y += h3_sx;
                den   += h6_sx;
                continue;
            }

            double sx = ( num_x / den ) * f_inv_scale_x;
            double sy = ( num_y / den ) * f_inv_scale_y;

            int i_sx = (int)( sx >= 0 ? sx : sx - 1 );
            int i_sy = (int)( sy >= 0 ? sy : sy - 1 );

            if( i_sx < -1 || i_sx >= i_src_width
             || i_sy < -1 || i_sy >= i_src_height )
            {
                p_out[x] = fill;
                num_x += h0_sx;
                num_y += h3_sx;
                den   += h6_sx;
                continue;
            }

            int i_fx = (int)( ( sx - i_sx ) * 256.0 );
            int i_fy = (int)( ( sy - i_sy ) * 256.0 );

            uint8_t p00 = fill, p10 = fill, p01 = fill, p11 = fill;

            if( i_sy >= 0 && i_sx >= 0 )
                p00 = p_src->p_pixels[i_sy * p_src->i_pitch + i_sx];
            if( i_sy >= 0 && i_sx + 1 < i_src_width )
                p10 = p_src->p_pixels[i_sy * p_src->i_pitch + i_sx + 1];
            if( i_sy + 1 < i_src_height && i_sx >= 0 )
                p01 = p_src->p_pixels[( i_sy + 1 ) * p_src->i_pitch + i_sx];
            if( i_sy + 1 < i_src_height && i_sx + 1 < i_src_width )
                p11 = p_src->p_pixels[( i_sy + 1 ) * p_src->i_pitch + i_sx + 1];

            unsigned int temp = 0;
            temp += p00 * ( 256 - i_fy ) * ( 256 - i_fx );
            temp += p01 * i_fy * ( 256 - i_fx );
            temp += p11 * i_fx * i_fy;
            temp += p10 * i_fx * ( 256 - i_fy );
            p_out[x] = temp >> 16;

            num_x += h0_sx;
            num_y += h3_sx;
            den   += h6_sx;
        }
    }
}

/*****************************************************************************
 * DrawHandle: draw a small filled square on the Y plane of the output picture
 *****************************************************************************/
static void DrawHandle( picture_t *p_pic, int i_cx, int i_cy,
                        int i_size, uint8_t y_val, uint8_t u_val,
                        uint8_t v_val )
{
    for( int i_plane = 0; i_plane < p_pic->i_planes && i_plane < 3; i_plane++ )
    {
        plane_t *p = &p_pic->p[i_plane];
        const int i_pw = p->i_visible_pitch / p->i_pixel_pitch;
        const int i_ph = p->i_visible_lines;
        const int i_yw = p_pic->p[Y_PLANE].i_visible_pitch
                         / p_pic->p[Y_PLANE].i_pixel_pitch;
        const int i_yh = p_pic->p[Y_PLANE].i_visible_lines;

        /* Scale center and size to this plane's coordinate space */
        int cx = i_cx * i_pw / i_yw;
        int cy = i_cy * i_ph / i_yh;
        int sz = i_size * i_pw / i_yw;
        if( sz < 1 ) sz = 1;

        int x0 = cx - sz; if( x0 < 0 ) x0 = 0;
        int y0 = cy - sz; if( y0 < 0 ) y0 = 0;
        int x1 = cx + sz; if( x1 > i_pw ) x1 = i_pw;
        int y1 = cy + sz; if( y1 > i_ph ) y1 = i_ph;

        uint8_t val = ( i_plane == Y_PLANE ) ? y_val
                    : ( i_plane == U_PLANE ) ? u_val : v_val;

        for( int y = y0; y < y1; y++ )
            memset( &p->p_pixels[y * p->i_pitch + x0], val, x1 - x0 );
    }
}

/*****************************************************************************
 * GetCornerPixelPos: compute the pixel position of a corner on the output
 *****************************************************************************/
static void GetCornerPixelPos( int i_corner, int i_width, int i_height,
                               float f_tl_x, float f_tl_y,
                               float f_tr_x, float f_tr_y,
                               float f_bl_x, float f_bl_y,
                               float f_br_x, float f_br_y,
                               int *pi_x, int *pi_y )
{
    switch( i_corner )
    {
        case 0: /* TL */
            *pi_x = (int)( f_tl_x * i_width );
            *pi_y = (int)( f_tl_y * i_height );
            break;
        case 1: /* TR */
            *pi_x = ( i_width - 1 ) + (int)( f_tr_x * i_width );
            *pi_y = (int)( f_tr_y * i_height );
            break;
        case 2: /* BL */
            *pi_x = (int)( f_bl_x * i_width );
            *pi_y = ( i_height - 1 ) + (int)( f_bl_y * i_height );
            break;
        case 3: /* BR */
            *pi_x = ( i_width - 1 ) + (int)( f_br_x * i_width );
            *pi_y = ( i_height - 1 ) + (int)( f_br_y * i_height );
            break;
        default:
            *pi_x = 0;
            *pi_y = 0;
            break;
    }
}

/*****************************************************************************
 * Create: allocate and initialize keystone filter
 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;

    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }

    switch( p_filter->fmt_in.video.i_chroma )
    {
        CASE_PLANAR_YUV
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)",
                     (char *)&p_filter->fmt_in.video.i_chroma );
            return VLC_EGENERIC;
    }

    filter_sys_t *p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_filter->p_sys = p_sys;

    config_ChainParse( p_filter, FILTER_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );

    /* Create persistent variables on the parent object so values survive
     * filter recreation (e.g., playlist loop). Pattern from ci_filters.m. */
    vlc_atomic_float *pf_all[] = {
        &p_sys->f_tl_x, &p_sys->f_tl_y, &p_sys->f_tr_x, &p_sys->f_tr_y,
        &p_sys->f_bl_x, &p_sys->f_bl_y, &p_sys->f_br_x, &p_sys->f_br_y,
    };
    for( int i = 0; i < 8; i++ )
    {
        const char *name = ppsz_corner_vars[i];
        /* Create on parent if not yet present (no-op if already exists) */
        var_Create( p_filter->obj.parent, name, VLC_VAR_FLOAT );
        /* Read persisted value from parent; if non-zero use it,
         * otherwise read from config/CLI via standard VLC inheritance */
        float parent_val = var_GetFloat( p_filter->obj.parent, name );
        float val;
        if( parent_val != 0.f )
            val = parent_val;
        else
            val = var_CreateGetFloatCommand( p_filter, name );
        vlc_atomic_init_float( pf_all[i], val );
        /* Also persist initial value to parent */
        var_SetFloat( p_filter->obj.parent, name, val );

        var_AddCallback( p_filter, name, KeystoneCallback, p_sys );
    }

    atomic_init( &p_sys->i_drag_corner, -1 );
    atomic_init( &p_sys->i_hover_corner, -1 );
    atomic_init( &p_sys->b_show_handles,
                 var_CreateGetBoolCommand( p_filter,
                                           FILTER_PREFIX "show-handles" ) );

    p_filter->pf_video_filter = Filter;
    p_filter->pf_video_mouse = Mouse;

    return VLC_SUCCESS;
}

/*****************************************************************************
 * Destroy: release keystone filter resources
 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;

    for( int i = 0; i < 8; i++ )
        var_DelCallback( p_filter, ppsz_corner_vars[i],
                         KeystoneCallback, p_sys );
    /* Note: parent variables are intentionally NOT destroyed so values
     * persist across filter recreation (playlist loop). */

    free( p_sys );
}

/*****************************************************************************
 * Filter: apply keystone transform and draw corner handles
 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_outpic;

    if( !p_pic )
        return NULL;

    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        msg_Warn( p_filter, "can't get output picture" );
        picture_Release( p_pic );
        return NULL;
    }

    /* Load current parameter values */
    float f_tl_x = vlc_atomic_load_float( &p_sys->f_tl_x );
    float f_tl_y = vlc_atomic_load_float( &p_sys->f_tl_y );
    float f_tr_x = vlc_atomic_load_float( &p_sys->f_tr_x );
    float f_tr_y = vlc_atomic_load_float( &p_sys->f_tr_y );
    float f_bl_x = vlc_atomic_load_float( &p_sys->f_bl_x );
    float f_bl_y = vlc_atomic_load_float( &p_sys->f_bl_y );
    float f_br_x = vlc_atomic_load_float( &p_sys->f_br_x );
    float f_br_y = vlc_atomic_load_float( &p_sys->f_br_y );

    /* Y plane dimensions */
    const int i_width  = p_pic->p[Y_PLANE].i_visible_pitch
                         / p_pic->p[Y_PLANE].i_pixel_pitch;
    const int i_height = p_pic->p[Y_PLANE].i_visible_lines;

    /* Identity short-circuit */
    if( f_tl_x == 0.f && f_tl_y == 0.f && f_tr_x == 0.f && f_tr_y == 0.f
     && f_bl_x == 0.f && f_bl_y == 0.f && f_br_x == 0.f && f_br_y == 0.f )
    {
        picture_Copy( p_outpic, p_pic );
    }
    else
    {
        /* Destination corners (where the user places them) */
        double dx0 = f_tl_x * i_width;
        double dy0 = f_tl_y * i_height;
        double dx1 = (double)( i_width - 1 ) + f_tr_x * i_width;
        double dy1 = f_tr_y * i_height;
        double dx2 = f_bl_x * i_width;
        double dy2 = (double)( i_height - 1 ) + f_bl_y * i_height;
        double dx3 = (double)( i_width - 1 ) + f_br_x * i_width;
        double dy3 = (double)( i_height - 1 ) + f_br_y * i_height;

        /* Source corners (original rectangle) */
        double sx0 = 0.0,                     sy0 = 0.0;
        double sx1 = (double)( i_width - 1 ), sy1 = 0.0;
        double sx2 = 0.0,                     sy2 = (double)( i_height - 1 );
        double sx3 = (double)( i_width - 1 ), sy3 = (double)( i_height - 1 );

        double h[8];
        if( !ComputeHomography( h,
                sx0, sy0, dx0, dy0,
                sx1, sy1, dx1, dy1,
                sx2, sy2, dx2, dy2,
                sx3, sy3, dx3, dy3 ) )
        {
            picture_Copy( p_outpic, p_pic );
            goto draw_handles;
        }

        for( int i = 0; i < p_pic->i_planes; i++ )
        {
            RenderPlane( &p_pic->p[i], &p_outpic->p[i],
                         i_width, i_height, h, i );
        }
    }

draw_handles:
    /* Draw handle for hovered or dragged corner only */
    if( atomic_load( &p_sys->b_show_handles ) )
    {
        int drag  = atomic_load( &p_sys->i_drag_corner );
        int hover = atomic_load( &p_sys->i_hover_corner );

        /* Pick which corner to show: drag takes priority over hover */
        int show = ( drag >= 0 ) ? drag : hover;
        if( show >= 0 && show < 4 )
        {
            int hx, hy;
            GetCornerPixelPos( show, i_width, i_height,
                               f_tl_x, f_tl_y, f_tr_x, f_tr_y,
                               f_bl_x, f_bl_y, f_br_x, f_br_y,
                               &hx, &hy );

            /* Clamp to visible area */
            if( hx < 0 ) hx = 0;
            if( hx >= i_width ) hx = i_width - 1;
            if( hy < 0 ) hy = 0;
            if( hy >= i_height ) hy = i_height - 1;

            if( drag >= 0 )
                DrawHandle( p_outpic, hx, hy, HANDLE_SIZE,
                            ACTIVE_Y, ACTIVE_U, ACTIVE_V );
            else
                DrawHandle( p_outpic, hx, hy, HANDLE_SIZE,
                            HOVER_Y, HOVER_U, HOVER_V );
        }
    }

    return CopyInfoAndRelease( p_outpic, p_pic );
}

/*****************************************************************************
 * Mouse: handle mouse interaction for corner dragging
 *****************************************************************************/
static int Mouse( filter_t *p_filter, vlc_mouse_t *p_mouse,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmt = &p_filter->fmt_out.video;
    const int i_width  = p_fmt->i_visible_width;
    const int i_height = p_fmt->i_visible_height;

    /* Load current offsets */
    float f_tl_x = vlc_atomic_load_float( &p_sys->f_tl_x );
    float f_tl_y = vlc_atomic_load_float( &p_sys->f_tl_y );
    float f_tr_x = vlc_atomic_load_float( &p_sys->f_tr_x );
    float f_tr_y = vlc_atomic_load_float( &p_sys->f_tr_y );
    float f_bl_x = vlc_atomic_load_float( &p_sys->f_bl_x );
    float f_bl_y = vlc_atomic_load_float( &p_sys->f_bl_y );
    float f_br_x = vlc_atomic_load_float( &p_sys->f_br_x );
    float f_br_y = vlc_atomic_load_float( &p_sys->f_br_y );

    if( i_width <= 0 || i_height <= 0 )
    {
        *p_mouse = *p_new;
        return VLC_SUCCESS;
    }

    /* Left button press: check if clicking on a corner handle */
    if( vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT ) )
    {
        int best = -1;
        int best_dist = HANDLE_SIZE * HANDLE_SIZE * 4;

        for( int c = 0; c < 4; c++ )
        {
            int hx, hy;
            GetCornerPixelPos( c, i_width, i_height,
                               f_tl_x, f_tl_y, f_tr_x, f_tr_y,
                               f_bl_x, f_bl_y, f_br_x, f_br_y,
                               &hx, &hy );

            int ddx = p_new->i_x - hx;
            int ddy = p_new->i_y - hy;
            int dist = ddx * ddx + ddy * ddy;

            if( dist < best_dist )
            {
                best_dist = dist;
                best = c;
            }
        }

        if( best >= 0 )
        {
            atomic_store( &p_sys->i_drag_corner, best );
            return VLC_EGENERIC;
        }
    }

    /* Dragging: update corner offset */
    int drag = atomic_load( &p_sys->i_drag_corner );

    if( vlc_mouse_IsLeftPressed( p_new ) && drag >= 0 )
    {
        int i_dx, i_dy;
        vlc_mouse_GetMotion( &i_dx, &i_dy, p_old, p_new );

        float f_dx = (float)i_dx / i_width;
        float f_dy = (float)i_dy / i_height;

        vlc_atomic_float *pf_x = NULL, *pf_y = NULL;
        const char *psz_x = NULL, *psz_y = NULL;

        switch( drag )
        {
            case 0: pf_x = &p_sys->f_tl_x; pf_y = &p_sys->f_tl_y;
                    psz_x = ppsz_corner_vars[0]; psz_y = ppsz_corner_vars[1];
                    break;
            case 1: pf_x = &p_sys->f_tr_x; pf_y = &p_sys->f_tr_y;
                    psz_x = ppsz_corner_vars[2]; psz_y = ppsz_corner_vars[3];
                    break;
            case 2: pf_x = &p_sys->f_bl_x; pf_y = &p_sys->f_bl_y;
                    psz_x = ppsz_corner_vars[4]; psz_y = ppsz_corner_vars[5];
                    break;
            case 3: pf_x = &p_sys->f_br_x; pf_y = &p_sys->f_br_y;
                    psz_x = ppsz_corner_vars[6]; psz_y = ppsz_corner_vars[7];
                    break;
        }

        if( pf_x && pf_y )
        {
            float new_x = vlc_atomic_load_float( pf_x ) + f_dx;
            float new_y = vlc_atomic_load_float( pf_y ) + f_dy;

            /* Clamp to range */
            if( new_x < -1.f ) new_x = -1.f;
            if( new_x >  1.f ) new_x =  1.f;
            if( new_y < -1.f ) new_y = -1.f;
            if( new_y >  1.f ) new_y =  1.f;

            vlc_atomic_store_float( pf_x, new_x );
            vlc_atomic_store_float( pf_y, new_y );

            /* Persist to parent for survival across filter recreation */
            var_SetFloat( p_filter->obj.parent, psz_x, new_x );
            var_SetFloat( p_filter->obj.parent, psz_y, new_y );
        }

        return VLC_EGENERIC;
    }

    /* Left button released: stop dragging */
    if( vlc_mouse_HasReleased( p_old, p_new, MOUSE_BUTTON_LEFT ) && drag >= 0 )
    {
        atomic_store( &p_sys->i_drag_corner, -1 );
        return VLC_EGENERIC;
    }

    /* Hover detection: highlight corner closest to cursor */
    {
        int best_hover = -1;
        int best_dist = HANDLE_SIZE * HANDLE_SIZE * 4;

        for( int c = 0; c < 4; c++ )
        {
            int hx, hy;
            GetCornerPixelPos( c, i_width, i_height,
                               f_tl_x, f_tl_y, f_tr_x, f_tr_y,
                               f_bl_x, f_bl_y, f_br_x, f_br_y,
                               &hx, &hy );

            int ddx = p_new->i_x - hx;
            int ddy = p_new->i_y - hy;
            int dist = ddx * ddx + ddy * ddy;

            if( dist < best_dist )
            {
                best_dist = dist;
                best_hover = c;
            }
        }

        atomic_store( &p_sys->i_hover_corner, best_hover );
    }

    /* No interaction: propagate mouse event */
    *p_mouse = *p_new;
    return VLC_SUCCESS;
}

/*****************************************************************************
 * KeystoneCallback: handle runtime parameter changes
 *****************************************************************************/
static int KeystoneCallback( vlc_object_t *p_this, char const *psz_var,
                             vlc_value_t oldval, vlc_value_t newval,
                             void *p_data )
{
    VLC_UNUSED( p_this ); VLC_UNUSED( oldval );
    filter_sys_t *p_sys = (filter_sys_t *)p_data;

    if( !strcmp( psz_var, FILTER_PREFIX "tl-x" ) )
        vlc_atomic_store_float( &p_sys->f_tl_x, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "tl-y" ) )
        vlc_atomic_store_float( &p_sys->f_tl_y, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "tr-x" ) )
        vlc_atomic_store_float( &p_sys->f_tr_x, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "tr-y" ) )
        vlc_atomic_store_float( &p_sys->f_tr_y, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "bl-x" ) )
        vlc_atomic_store_float( &p_sys->f_bl_x, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "bl-y" ) )
        vlc_atomic_store_float( &p_sys->f_bl_y, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "br-x" ) )
        vlc_atomic_store_float( &p_sys->f_br_x, newval.f_float );
    else if( !strcmp( psz_var, FILTER_PREFIX "br-y" ) )
        vlc_atomic_store_float( &p_sys->f_br_y, newval.f_float );

    return VLC_SUCCESS;
}
