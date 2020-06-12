<p align="center">
	<img src="logo.svg" width="30%">
</p>

<div align="center">
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=bugs'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=code_smells'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=coverage'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=duplicated_lines_density'/>
</div>

<div align="center">
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=ncloc'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=sqale_rating'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=alert_status'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=reliability_rating'/>
</div>

<div align="center">
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=security_rating'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=sqale_index'/>
    <img src='https://sonar.x99.fr/api/project_badges/measure?project=NMEAParser&metric=vulnerabilities'/>
</div>

NMEAParser is a library that aims at drastically simplifiying the parsing of [NMEA](https://www.wikiwand.com/en/NMEA_0183) sentences. These can be from a GPS, a gyrocompass or any device that complies with the NMEA 0183 standard.

# But...why?

The reason why I wanted to write this library as there is many like this one, like TinyGPS (and its numerous forks/rewrites)? Because of their adaptability. Have you ever tried to add a custom string to TinyGPS? I did, and it was a real pain in the ass. So I decided to develop my own lib, making it easy to extend. It relies heavily on a custom implementation of sscanf to fit my needs. Technically speaking, `sscanf`
parses very wells NMEA strings, but has two main drawbacks:
- it's ways to sophisticated for what we need here. And some datas such as latitude or UTC time are recognized as floats.
- `sscanf` doesn't support empty fields. In NMEA strings, blank fields are common, especially when GPS works indoor. Default `sscanf` sees empty fields as errors. Mine parses them fine.

Beyond these considerations, NMEAParser is damn fast and has a very low memory footprint. This is what you need when working with Arduinos, ESPs and such.

# Technical details
## Performance
To assess performance, see unit tests. A part of these is dedicated to performance. Basically, `NMEAParser` was used to dispatch and parse great amounts of sentences. Results are very linear, according to the results.

The results are the following:

### Arduino Mega 2560, 16MHz

Number of sentences | Time in seconds
-|-
150 | 0.059
1 500 | 0.591
15 000 | 5.915
150k | 59.155

### ESP32, 240MHz, 1 core

Number of sentences | Time in seconds
-|-
150 | 0.002
1 500 | 0.025
15 000 | 0.254
150k | 2.530
1.5M | 25.288

### Your own computer

NMEAParser now features native tests, meaning that you can benchmark the library using your own computer, not just a microcontroller. On my i7-6700K, 1.5 millions NMEA strings were parsed in approx. 225ms.


## Adding your own sentences

It's quite simple to add your own NMEA sentence:
- declare its structure, with a default `bool isValid` and `int16_t fieldValidity` inside it.
- declare its type in `STRINGS_TYPES`
- create a parsing function based on existing ones
- declare your sentence type in the `dispatch` function.
- you're done!

## References

- [NMEA Reference Manual](https://www.sparkfun.com/datasheets/GPS/NMEA%20Reference%20Manual-Rev2.1-Dec07.pdf)
- [NMEA Sentences Reference](https://www.gpsinformation.org/dale/nmea.htm)
- [NMEA 0183 Amendment (adds $GPTXT)](https://www.nmea.org/Assets/20160520%20txt%20amendment.pdf)

# Todo
- [ ] Add examples
- [ ] Complete unit tests
- [ ] Add way to compose sentences
- [ ] Add calculation tools (distances..)

# Copyright
## Logo
Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a>