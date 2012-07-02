{-# LANGUAGE CPP, ForeignFunctionInterface #-}
{-# OPTIONS_GHC -fno-warn-missing-signatures #-}

-- -*-haskell-*-
--  GIMP Toolkit (GTK) CellRendererCairo
--
--  Author : John W. Lato
--
--  Created: June 2012
--
--  Copyright (C) 2012 Tsuru Capital SG LLC
--
--  This library is free software; you can redistribute it and/or
--  modify it under the terms of the GNU Lesser General Public
--  License as published by the Free Software Foundation; either
--  version 2.1 of the License, or (at your option) any later version.
--
--  This library is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
--  Lesser General Public License for more details.
--
-- |
-- Maintainer  : jwlato@gmail.com
-- Stability   : provisional
-- Portability : portable (depends on GHC)
--
-- Renders a cell
--
module Graphics.UI.Gtk.ModelView.CellRendererCairo (
-- * Detail
--
-- | A 'CellRendererCairo' can be used to render an image in a cell. It
-- allows to render via an arbitrary 'Render ()' command.
--

-- * Class Hierarchy
-- |
-- @
-- |  'GObject'
-- |   +----'Object'
-- |         +----'CellRenderer'
-- |               +----CellRendererCairo
-- @

-- * Types
  CellRendererCairo,
  CellRendererCairoClass,
  castToCellRendererCairo, gTypeCellRendererCairo,
  toCellRendererCairo,
  withCellRendererCairo,

-- * Constructors
  cellRendererCairoNew,

-- * Functions
  cellRendererSetRenderer,

-- * Attributes
  cellRenderer
  ) where

import Control.Monad	(liftM, unless)
import Control.Monad.Reader (runReaderT)

import System.Glib.FFI
import System.Glib.GType
import System.Glib.Types
import Graphics.UI.Gtk.Abstract.Object		(makeNewObject)
import Graphics.UI.Gtk
import Graphics.Rendering.Cairo.Internal

#include "gtkcellrenderercairo.h"

{# context lib="gtk" prefix="gtk" #}

-- ----------------------------------------------------
-- CellRendererCairo type binding/boilerplate


-- | A Cairo-based CellRenderer
{#pointer *CellRendererCairo foreign newtype #}

mkCellRendererCairo = (CellRendererCairo, objectUnrefFromMainloop)
unCellRendererCairo (CellRendererCairo o) = o

class CellRendererClass o => CellRendererCairoClass o
toCellRendererCairo :: CellRendererCairoClass o => o -> CellRendererCairo
toCellRendererCairo = unsafeCastGObject . toGObject

instance CellRendererCairoClass CellRendererCairo
instance CellRendererClass CellRendererCairo
instance ObjectClass CellRendererCairo
instance GObjectClass CellRendererCairo where
  toGObject = GObject . castForeignPtr . unCellRendererCairo
  unsafeCastGObject = CellRendererCairo . castForeignPtr . unGObject

castToCellRendererCairo :: GObjectClass obj => obj -> CellRendererCairo
castToCellRendererCairo = castTo gTypeCellRendererCairo "CellRendererCairo"

gTypeCellRendererCairo :: GType
gTypeCellRendererCairo =
      {# call fun unsafe gtk_cell_renderer_cairo_get_type #}


--------------------
-- Constructors

-- | Create a new CellRendererCairo object.
--
cellRendererCairoNew :: IO CellRendererCairo
cellRendererCairoNew = do
  obj <- makeNewObject mkCellRendererCairo $
    liftM castPtr $ {# call unsafe cell_renderer_cairo_new #}
  return obj

--------------------
-- Attributes

-- | The current rendering function.
cellRenderer
  :: CellRendererCairoClass self => WriteAttr self (Rectangle -> Render ())
cellRenderer = writeAttr cellRendererSetRenderer


--------------------
-- marshallers

-- | set the rendering function used by a 'CellRendererCairo'
cellRendererSetRenderer
  :: CellRendererCairoClass self => self -> (Rectangle -> Render ()) -> IO ()
cellRendererSetRenderer self callback = do
      -- closure args
      --  Cairo Ptr
      --  x
      --  y
      --  width
      --  height
  sptr <- newStablePtr cb
  gclosurePtr <- gtk2hs_closure_new sptr
  withForeignPtr (unGObject $ toGObject self)
      (\p -> set_cell_renderer (castPtr p) gclosurePtr)
  where
      cb cptr x y width height = do
          let rect = Rectangle x y width height
              context = Cairo cptr
          runReaderT (runRender (callback rect)) context
          status context >>= \stat -> unless
              (stat == StatusSuccess) $ fail =<< statusToString stat

--------------------
-- foreign code
--

foreign import ccall "gtk_cell_renderer_cairo_set_cell_renderer"
  set_cell_renderer :: Ptr () -> Ptr GClosure -> IO ()

-- this stuff isn't exported by gtk2hs, so we need to define it again....

foreign import ccall unsafe "gtk2hs_closure_new"
  gtk2hs_closure_new :: StablePtr a -> IO (Ptr GClosure)

foreign import ccall unsafe "hsgthread.h &gtk2hs_g_object_unref_from_mainloop"
  objectUnrefFromMainloop :: FinalizerPtr a

castTo :: (GObjectClass obj, GObjectClass obj') => GType -> String
                                                -> (obj -> obj')
castTo gtype objTypeName obj =
  case toGObject obj of
    gobj@(GObject objFPtr)
      | typeInstanceIsA ((unsafeForeignPtrToPtr.castForeignPtr) objFPtr) gtype
            -> unsafeCastGObject gobj
      | otherwise -> error $ "Cannot cast object to " ++ objTypeName 
