-- premake5.lua
workspace "MABRender"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "MABRender"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "MABRender"