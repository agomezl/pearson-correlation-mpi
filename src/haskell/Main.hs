{-# LANGUAGE UnicodeSyntax #-}
{-# LANGUAGE ScopedTypeVariables #-}
--------------------------------------------------------------------------------
-- File   : Main
-- Author : Alejandro Gómez Londoño
-- Date   : Mon Jun  2 20:40:06 2014
-- Description : haskell version
--------------------------------------------------------------------------------
-- Change log :

--------------------------------------------------------------------------------


module Main where

import System.Environment (getArgs)

main ∷ IO ()
main = do
  (path:_) ← getArgs
  file ← readFile path
  let matrix ∷ [[Int]] = do
        line ← lines file
        let vals = map (read) $ words line
        return vals
  print matrix
  return ()
