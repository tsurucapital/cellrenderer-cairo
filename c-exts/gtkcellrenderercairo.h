/* gtkcellrenderercairo.h
 * Copyright (C) 2012  Tsuru Capital LLC
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


#ifndef __GTK_CELL_RENDERER_CAIRO_H__
#define __GTK_CELL_RENDERER_CAIRO_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS


#define GTK_TYPE_CELL_RENDERER_CAIRO			(gtk_cell_renderer_cairo_get_type ())
#define GTK_CELL_RENDERER_CAIRO(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CELL_RENDERER_CAIRO, GtkCellRendererCairo))
#define GTK_CELL_RENDERER_CAIRO_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_CELL_RENDERER_CAIRO, GtkCellRendererCairoClass))
#define GTK_IS_CELL_RENDERER_CAIRO(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CELL_RENDERER_CAIRO))
#define GTK_IS_CELL_RENDERER_CAIRO_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_CELL_RENDERER_CAIRO))
#define GTK_CELL_RENDERER_CAIRO_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CELL_RENDERER_CAIRO, GtkCellRendererCairoClass))


typedef struct _GtkCellRendererCairo GtkCellRendererCairo;
typedef struct _GtkCellRendererCairoClass GtkCellRendererCairoClass;

struct _GtkCellRendererCairo
{
  GtkCellRenderer parent;

  /*< private >*/
  GClosure *cairo_render;
};

struct _GtkCellRendererCairoClass
{
  GtkCellRendererClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};

GType            gtk_cell_renderer_cairo_get_type (void) G_GNUC_CONST;
GtkCellRenderer *gtk_cell_renderer_cairo_new      (void);

void gtk_cell_renderer_cairo_set_cell_renderer(
    GtkCellRendererCairo *obj,
    GClosure *renderer);

G_END_DECLS


#endif /* __GTK_CELL_RENDERER_CAIRO_H__ */
