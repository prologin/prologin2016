{-# LANGUAGE ForeignFunctionInterface #-}

-- This file has been generated, if you wish to
-- modify it in a permanent way, please refer
-- to the script file : gen/generator_haskell.rb

module Prologin where

import Api
import Control.Monad
import Data.Bool
import Data.Foldable
import Data.Function
import System.IO.Unsafe

sFlip, sFlip', sRotate, f :: Position -> Position
sFlip (Position x y) = Position y x
sFlip' (Position x y) = Position (taille_terrain - y - 1) (taille_terrain - x - 1)
sRotate (Position x y) = Position (taille_terrain - y - 1) x

f = unsafePerformIO $ do
  ps <- liste_pulsars
  let accept f = foldr (\p a -> est_pulsar (f p) >>= bool (return False) a) (return True) ps
  fs <- filterM accept [sFlip, sFlip']
  return (head $ fs ++ [sFlip])

try_clear = void . deblayer . f
try_build = void . construire . f
try_upgrade = void . ameliorer . f
try_destroy = void . detruire . f

try_move_upa = deplacer_aspiration `on` f

jouer_tour :: IO ()
jouer_tour = do
  hist_debris_deblayes >>= traverse_ try_clear
  hist_tuyaux_construits >>= traverse_ try_build
  hist_tuyaux_ameliores >>= traverse_ try_upgrade
  hist_tuyaux_detruits >>= traverse_ try_destroy
  liftM2 zip hist_points_aspiration_retires hist_points_aspiration_ajoutes
    >>= traverse_ (uncurry try_move_upa)

hs_partie_init = return ()
foreign export ccall hs_partie_init :: IO ()
hs_jouer_tour = jouer_tour
foreign export ccall hs_jouer_tour :: IO ()
hs_partie_fin = return ()
foreign export ccall hs_partie_fin :: IO ()
