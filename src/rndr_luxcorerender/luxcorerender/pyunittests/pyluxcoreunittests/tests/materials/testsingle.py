# -*- coding: utf-8 -*-
################################################################################
# Copyright 1998-2018 by authors (see AUTHORS.txt)
#
#   This file is part of LuxCoreRender.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

import unittest
import pyluxcore

from pyluxcoreunittests.tests.utils import *
from pyluxcoreunittests.tests.imagetest import *

# Note: keep in alphabetical order

################################################################################
# ArchGlass material test
################################################################################

def TestArchGlassMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-archglass-area.cfg", "ArchGlassMaterial")

class ArchGlassMaterial(ImageTest):
    pass

ArchGlassMaterial = AddTests(ArchGlassMaterial, TestArchGlassMaterial, GetTestCases())

################################################################################
# CarPaint material test
################################################################################

def TestCarPaintMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-carpaint-area.cfg", "CarPaintMaterial")

class CarPaintMaterial(ImageTest):
    pass

CarPaintMaterial = AddTests(CarPaintMaterial, TestCarPaintMaterial, GetTestCases())

################################################################################
# Cloth material test
################################################################################

def TestClothMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-cloth-area.cfg", "ClothMaterial")

class ClothMaterial(ImageTest):
    pass

ClothMaterial = AddTests(ClothMaterial, TestClothMaterial, GetTestCases())

################################################################################
# Disney material test
################################################################################

def TestDisneyMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-disney-area.cfg", "DisneyMaterial")

class DisneyMaterial(ImageTest):
    pass

DisneyMaterial = AddTests(DisneyMaterial, TestDisneyMaterial, GetTestCases())

################################################################################
# Glass material test
################################################################################

def TestGlassMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-glass-area.cfg", "GlassMaterial")

class GlassMaterial(ImageTest):
    pass

GlassMaterial = AddTests(GlassMaterial, TestGlassMaterial, GetTestCases())

################################################################################
# Glass+Dispersion material test
################################################################################

def TestGlassDispMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-glass-disp-area.cfg", "GlassDispMaterial")

class GlassDispMaterial(ImageTest):
    pass

GlassDispMaterial = AddTests(GlassDispMaterial, TestGlassDispMaterial, GetTestCases())

################################################################################
# Glossy2 material test
################################################################################

def TestGlossy2Material(cls, params):
	StandardSceneTest(cls, params, "simple/mat-glossy2-area.cfg", "Glossy2Material")

class Glossy2Material(ImageTest):
    pass

Glossy2Material = AddTests(Glossy2Material, TestGlossy2Material, GetTestCases())

################################################################################
# GlossyCoating material test
################################################################################

def TestGlossyCoatingMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-glossycoating-area.cfg", "GlossyCoatingMaterial")

class GlossyCoatingMaterial(ImageTest):
    pass

GlossyCoatingMaterial = AddTests(GlossyCoatingMaterial, TestGlossyCoatingMaterial, GetTestCases())

################################################################################
# GlossyTranslucent material test
################################################################################

def TestGlossyTranslucentMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-glossytranslucent-area.cfg", "GlossyTranslucentMaterial")

class GlossyTranslucentMaterial(ImageTest):
    pass

GlossyTranslucentMaterial = AddTests(GlossyTranslucentMaterial, TestGlossyTranslucentMaterial, GetTestCases())

################################################################################
# Matte material test
################################################################################

def TestMatteMaterial(cls, params):
	StandardSceneTest(cls, params, "simple/mat-matte-area.cfg", "MatteMaterial")

class MatteMaterial(ImageTest):
    pass

MatteMaterial = AddTests(MatteMaterial, TestMatteMaterial, GetTestCases())
