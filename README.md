---
documentclass: physycomen
title:  "datalogger"
author: "Fabbri, Sinigardi"
---

<a href="http://www.physycom.unibo.it"> 
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna"> 
</div>
</a>
<a href="https://travis-ci.com/physycom/datalogger"> 
<div class="image">
<img src="https://travis-ci.com/physycom/datalogger.svg?token=ujjUseBa9hYbKckXBkxJ&branch=master" width="90" height="20" alt="Build Status"> 
</div>
</a>

## Purpose
This tool acquires data from different brands of black boxes through the serial port, using a wrapper around boost::asio, and dumps them into raw files on disk. It can also plot data in realtime using an FLTK widget.

## Installation
**make** and a **C++11** compatible compiler are required. Clone the repo and type ``make``, it should be enough in most cases!   
There's also a **VS2015** solution avalaible.   
