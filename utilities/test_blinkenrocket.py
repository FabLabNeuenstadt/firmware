#!/usr/bin/python
import unittest
from blinkenrocket import *

class TestFrame(unittest.TestCase):

  def test_notImplemented(self):
      frame = Frame()
      with self.assertRaises(NotImplementedError):
        frame.getRepresentation()

class TestAnimation(unittest.TestCase):

  def test_speedDefault(self):
    anim = animationFrame([])
    self.assertEquals(ord(anim.getHeader()[0]),1)

  def test_speedOkay(self):
    anim = animationFrame([],speed=7)
    self.assertEquals(ord(anim.getHeader()[0]),7)

  def test_speedNotOkay(self):
    anim = animationFrame([],speed=70)
    self.assertEquals(ord(anim.getHeader()[0]),1)

  def test_delayDefault(self):
    anim = animationFrame([])
    self.assertEquals(ord(anim.getHeader()[1]),0)

  def test_delayOkay(self):
    anim = animationFrame([],delay=7)
    self.assertEquals(ord(anim.getHeader()[1]),7)

  def test_delayNotOkay(self):
    anim = animationFrame([],delay=70)
    self.assertEquals(ord(anim.getHeader()[1]),0)

  def test_illegalLength(self):
    with self.assertRaises(Exception):
      anim = animationFrame([0x11,0x12,0x13,0x14])

  def test_allowedLength(self):
    anim = animationFrame([0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18])

  def test_defaultHeaderOK(self):
    anim = animationFrame([])
    self.assertEquals(anim.getHeader(),[chr(1),chr(0)])

  def test_differentHeaderOK(self):
    anim = animationFrame([],speed=7,delay=8)
    self.assertEquals(anim.getHeader(),[chr(7),chr(8)])

if __name__ == '__main__':
    unittest.main()