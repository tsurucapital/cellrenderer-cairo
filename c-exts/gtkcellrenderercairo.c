/* gtkcellrenderercairo.c
 * Copyright (C) 2000  Tsuru Capital
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <stdlib.h>
#include <cairo/cairo.h>
#include "gtkcellrenderercairo.h"

static void gtk_cell_renderer_cairo_get_property  (GObject                    *object,
						    guint                       param_id,
						    GValue                     *value,
						    GParamSpec                 *pspec);
static void gtk_cell_renderer_cairo_set_property  (GObject                    *object,
						    guint                       param_id,
						    const GValue               *value,
						    GParamSpec                 *pspec);
static void gtk_cell_renderer_cairo_finalize   (GObject                    *object);
static void gtk_cell_renderer_cairo_get_size   (GtkCellRenderer            *cell,
						 GtkWidget                  *widget,
						 GdkRectangle               *rectangle,
						 gint                       *x_offset,
						 gint                       *y_offset,
						 gint                       *width,
						 gint                       *height);
static void gtk_cell_renderer_cairo_render     (GtkCellRenderer            *cell,
						 GdkDrawable                *window,
						 GtkWidget                  *widget,
						 GdkRectangle               *background_area,
						 GdkRectangle               *cell_area,
						 GdkRectangle               *expose_area,
						 GtkCellRendererState        flags);


enum {
  PROP_0,
  PROP_CELL_RENDERER,
};

G_DEFINE_TYPE (GtkCellRendererCairo, gtk_cell_renderer_cairo, GTK_TYPE_CELL_RENDERER)

static void
gtk_cell_renderer_cairo_init (GtkCellRendererCairo *cellcairo)
{
   /* in pixbuf, this initialized a private struct.  Don't know yet if we'll
    * need it here
    */
}

static void
gtk_cell_renderer_cairo_class_init (GtkCellRendererCairoClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);

  object_class->finalize = gtk_cell_renderer_cairo_finalize;

  object_class->get_property = gtk_cell_renderer_cairo_get_property;
  object_class->set_property = gtk_cell_renderer_cairo_set_property;

  cell_class->get_size = gtk_cell_renderer_cairo_get_size;
  cell_class->render = gtk_cell_renderer_cairo_render;

  g_object_class_install_property (object_class,
				   PROP_CELL_RENDERER,
				   g_param_spec_pointer ("cellRenderer",
							"Cell rendering function",
							"The callback function used to render a cell",
							G_PARAM_READWRITE));

}

static void
gtk_cell_renderer_cairo_finalize (GObject *object)
{
  GtkCellRendererCairo *cellcairo = GTK_CELL_RENDERER_CAIRO (object);

  if (cellcairo->cairo_render)
    g_closure_unref (cellcairo->cairo_render);

  G_OBJECT_CLASS (gtk_cell_renderer_cairo_parent_class)->finalize (object);
}

static void
gtk_cell_renderer_cairo_get_property (GObject        *object,
				       guint           param_id,
				       GValue         *value,
				       GParamSpec     *pspec)
{
  GtkCellRendererCairo *cellcairo = GTK_CELL_RENDERER_CAIRO (object);

  switch (param_id)
    {
    case PROP_CELL_RENDERER:
      g_value_set_object (value, cellcairo->cairo_render);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

static void
gtk_cell_renderer_cairo_set_property (GObject      *object,
				       guint         param_id,
				       const GValue *value,
				       GParamSpec   *pspec)
{
  GtkCellRendererCairo *cellcairo = GTK_CELL_RENDERER_CAIRO (object);

  switch (param_id)
    {
    case PROP_CELL_RENDERER:
      if (cellcairo->cairo_render)
	      g_closure_unref (cellcairo->cairo_render);
      cellcairo->cairo_render = (GClosure*) g_value_dup_object (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
    }
}

void gtk_cell_renderer_cairo_set_cell_renderer(
      GtkCellRendererCairo *cellcairo,
      GClosure *renderer)
{
  if (cellcairo)
  {
    if (cellcairo->cairo_render)
	      g_closure_unref (cellcairo->cairo_render);

    cellcairo->cairo_render = g_closure_ref(renderer);
  }
}

/**
 * gtk_cell_renderer_cairo_new:
 * 
 * Creates a new #GtkCellRendererCairo. Adjust rendering
 * parameters using object properties. Object properties can be set
 * globally (with g_object_set()). Also, with #GtkTreeViewColumn, you
 * can bind a property to a value in a #GtkTreeModel.
 * 
 * Return value: the new cell renderer
 **/
GtkCellRenderer *
gtk_cell_renderer_cairo_new (void)
{
  GtkCellRenderer *cellcairo;

  cellcairo = g_object_new (GTK_TYPE_CELL_RENDERER_CAIRO, NULL);

  return cellcairo;
}


static void
gtk_cell_renderer_cairo_get_size (GtkCellRenderer *cell,
				   GtkWidget       *widget,
				   GdkRectangle    *cell_area,
				   gint            *x_offset,
				   gint            *y_offset,
				   gint            *width,
				   gint            *height)
{
  GtkCellRendererCairo *cellcairo = (GtkCellRendererCairo *) cell;

  if (cell_area)
  {

    if (x_offset)
	  {
	    *x_offset = cell->xalign * cell->xpad;
	    *x_offset = MAX (*x_offset, 0);
	  }
    if (y_offset)
	  {
	    *y_offset = cell->yalign * cell->ypad;
      *y_offset = MAX (*y_offset, 0);
	  }
    if (width)
     *width = cell_area->x - cell->xpad * 2;
  
    if (height)
      *height = cell_area->y - cell->ypad *2;
  }
  else
    {
      if (x_offset) *x_offset = 0;
      if (y_offset) *y_offset = 0;
      if (width)    *width    = 0;
      if (height)   *height   = 0;
    }
}

static void
gtk_cell_renderer_cairo_render (GtkCellRenderer      *cell,
				 GdkWindow            *window,
				 GtkWidget            *widget,
				 GdkRectangle         *background_area,
				 GdkRectangle         *cell_area,
				 GdkRectangle         *expose_area,
				 GtkCellRendererState  flags)

{
  GtkCellRendererCairo *cellcairo = (GtkCellRendererCairo *) cell;

  GClosure *render_function;

  GdkRectangle draw_rect;
  GValue cairo_args[5] = { { 0, }, { 0, }, { 0, }, { 0, }, { 0, } };
  GValue *arg1;
  GValue *arg;
  cairo_t *cr;
  int ix;
  cairo_status_t stat;

  arg1 = cairo_args;

  draw_rect = *cell_area;
  if ( !gdk_rectangle_intersect (expose_area, &draw_rect, &draw_rect))
     return;

  render_function = cellcairo->cairo_render;

  if (!render_function)
    return;

  cr = gdk_cairo_create (window);

  g_value_init(arg1, G_TYPE_POINTER);
  g_assert (G_VALUE_HOLDS(arg1, G_TYPE_POINTER));
  g_value_set_pointer(arg1, (gpointer) cr);

  arg=cairo_args+1;
  g_value_init(arg, G_TYPE_INT);
  g_assert (G_VALUE_HOLDS(arg, G_TYPE_INT));
  g_value_set_int(arg, draw_rect.x);

  arg++;
  g_value_init(arg, G_TYPE_INT);
  g_assert (G_VALUE_HOLDS(arg, G_TYPE_INT));
  g_value_set_int(arg, draw_rect.y);

  arg++;
  g_value_init(arg, G_TYPE_INT);
  g_assert (G_VALUE_HOLDS(arg, G_TYPE_INT));
  g_value_set_int(arg, draw_rect.width);

  arg++;
  g_value_init(arg, G_TYPE_INT);
  g_assert (G_VALUE_HOLDS(arg, G_TYPE_INT));
  g_value_set_int(arg, draw_rect.height);

  g_closure_invoke(render_function, NULL,5, cairo_args, NULL);
  
  for(ix=4;ix>=0;ix--)
  {
      g_value_unset(cairo_args+ix);
  }

  cairo_destroy (cr);

}

#define __GTK_CELL_RENDERER_CAIRO_C__
