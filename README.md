SZARP - System Zbierania Archiwizacji i Prezentacji danych
==========================================================

#### http://www.szarp.org/ ####

*Copyright © 1991-2010 Witold Kowalewski Praterm Automatyzacja Ciepłownictwa*  
*Copyright © 1995-2009 Praterm S.A.*  
*Copyright © 1991-2015 various authors (see [AUTHORS](AUTHORS) file)*  

Table of contents
-----------------

1.  What is SZARP?
2.  What does SZARP mean? How to pronounce it?
3.  What are the features of SZARP?
4.  What is the origin of SZARP?
5.  On what platform is SZARP running?
6.  How to install SZARP?
7.  Where can I find documentation?
8.  How can I get SZARP sources?
9.  How to setup my first SZARP server?
10. How to get help?


1. What is SZARP?
-----------------

**SZARP** is a full-featured SCADA system designed for monitoring slowly
changing industrial processes, such as operations of *urban heating systems*.
It is completely free software, published under terms of the *GNU General
Public License 2.0* (or any later version).


2. What does SZARP mean? How to pronounce it?
---------------------------------------------

**SZARP** is an acronym for a Polish name: *System Zbierania ARchwizacji i
Prezentacji danych*, which means *System for Data Acquisition, Archiving and
Presentation*. Digraph "sz" in Polish is pronounced similarly to English "sh",
so "SZARP" should be pronounced as a word "sharp".


3. What are the features of SZARP?
----------------------------------

**SZARP** architecture organized into three layers.

The first layer are **device drivers**. There are several drivers available,
including:

* Modbus RTU/TCP (master and slave)
* MBus
* Praterm ZET/Sterkom PLC
* Pollustat, Infocal and Kamstrup heating meters
* DDE (Dynamic Data Exchange) protocol
* communication with Python scripts through IPC
* importing data from text files and text output of programs

These device drivers are called *line daemons* (for historical reasons).

The second layer consist of **data colleting daemons** that read data from
drivers, calculate average probes and formulas (for *defined parameters*)
and finally save data to a SZARP database. Probes are generally available with
10-seconds resolution, but database contains 10-minutes average values. There
is also a web server available for serving parameters' values through HTTP.

In the third layer there are **client programs**, including:
 
* *Reporter* - a program for viewing current (10-seconds averages) values of
  parameters.
* *Controller* - a program for signalizing parameter's values irregularities.
* *Extractor* - a program for exporting data from databases to spreadsheet
  files.
* *Draw* - a powerful tool for viewing and analyzing historical data.


4. What is the origin of SZARP?
-------------------------------

**SZARP** was developed since 1991 (sic!) by *Praterm* company and was used in
all heating systems (over 20) owned by *Praterm* and that much other systems
all over Poland. In 2007 *Praterm* decided to publish almost all of the code
under GNU GPL. Nowadays **SZARP** is developed as Open Source project on
[GitHub](http://github.com/).

Project website is located at http://www.szarp.org/. Today, the main founder of
SZARP development is *Newterm* company (see http://newterm.pl/).


5. On what platform is SZARP running?
-------------------------------------

**SZARP** is developed and deployed for *Debian GNU/Linux*, but should compile
and run on any modern Linux distribution on *x86*, *amd64* and *armel/armhf*
architectures. Client applications can also be run on *Windows XP/Vista/7*
machines with NTFS filesystem.


6. How to install SZARP?
------------------------

File [INSTALL](INSTALL) contains installation instructions.


7. Where can I find documentation?
----------------------

Unluckily most of the user documentation is available only in Polish.  Sources
of documentation are in
[resources/documentation/new/](resources/documentation/new/) project's
directory and available online at http://www.szarp.org/. Most of the new code
is pretty-well documented (in English) using *Doxygen*.


8. How can I get SZARP sources?
-------------------------------

See http://www.szarp.org/en/download.


9. How to setup my first SZARP server?
---------------------------------------

First install *szarp-server* and *szarp-wx* packages (for details see
[INSTALL](INSTALL)):

```sh
# apt-get install szarp-server szarp-wx
```

In a configuration stage, you have to set 4-letter name of a configuration
(called *configuration prefix*), for example use "test". Answer "yes" to a
question about creating `szbase` directory.

Now look at the `/etc/szarp` directory. There are two files: 

 * `parstart.cfg` - this files defines what SZARP services should be run on
   system start. In default behaviour computer is considered as SZARP server
   only when the hostname equals to a configuration prefix - if that so,
   appropriate services will be run. This is probably not your case, so modify
   the file that it contains:
```
PARCOOK=1
MEANER3=1
SENDER=1
PARAMD=1
PSETD=0
XSLTD=0
SSS=0
```
 * `szarp.cfg` - this is a SZARP system configuration file. There are a lot of
   available options, but for now you only need to set *configuration prefix*.
   Modify the beginning of the file so it looks like this:
```bash
$if $prefix$=""
$prefix$:="test"
# $prefix$:=exec("hostname -s")
$end
```
(`$prefix$` variable should be set to 'test')

If all above is done, you have to prepare SZARP parameters configuration. We
will make configuration with one line daemon that polls system for average
load. Go to the directory `/opt/szarp/test` and create subdirectory called
`config`. Place there file with name `params.xml` and the following content:
```xml
<?xml version="1.0"?>
<params xmlns="http://www.praterm.com.pl/SZARP/ipk" xmlns:exec="http://www.praterm.com.pl/SZARP/ipk-extra" version="1.0" read_freq="10" send_freq="10" title="Test configuration">
  <device daemon="/opt/szarp/bin/execdmn" path="/opt/szarp/test/get_la.sh" exec:frequency="10">
    <unit id="1" type="1" subtype="1" bufsize="1">
      <param name="Test:System:average system load" short_name="lavg" unit="-" prec="2" base_ind="auto">
        <raport title="System report"/>
        <draw title="System" min="0" max="100"/>
      </param>
    </unit>
  </device>
</params>
```
Then in directory `/opt/szarp/test/config` run command:
```sh
 $ /opt/szarp/bin/i2smo
```
(this parses XML configuration, if every thing is ok, you should see "OK")

Prepare executable script `/opt/szarp/test/get_la.sh` with the following
content:
```bash
	#!/bin/bash
	cat /proc/loadavg | cut -d ' ' -f 1 | tr -d '.'
```
Save it and then restart SZARP services:
```sh
# /etc/init.d/parstart restart
```

And that's all, you should be able to point your browser to
http://localhost:8081/ and see your configured parameter. After full 10 minutes
you can also run *Draw3* program:
```sh
 $ /opt/szarp/bin/draw3
```
and analyze your system average load.


10. How to get help?
--------------------

You can try to write us at <coders@newterm.pl>.

