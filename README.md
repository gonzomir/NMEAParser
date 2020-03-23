<p align="center">
	<img src="logo.svg" width="30%">
</p>

NMEAParser is a library that aims at drastically simplifiying the parsing of [NMEA](https://www.wikiwand.com/en/NMEA_0183) sentences. These can be from a GPS, a gyrocompass or any device that complies with the NMEA 0183 standard.

# But...why?

The reason why I wanted to write this library as there is many like this one, like TinyGPS (and its numerous forks/rewrites)? Because of their adaptability. Have you ever tried to add a custom string to TinyGPS? I did, and it was a real pain in the ass. So I decided to develop my own lib, making it easy to extend. It relies heavily on a custom implementation of sscanf to fit my needs. Technically speaking, `sscanf`
parses very wells NMEA strings, but has two main drawbacks:
- it's ways to sophisticated for what we need here. And some datas such as latitude or UTC time are recognized as floats.
- `sscanf` doesn't support empty fields. In NMEA strings, blank fields are common, especially when GPS works indoor. Default `sscanf` sees empty fields as errors. Mine parses them fine.

Beyond these considerations, NMEAParser is damn fast and has a very low memory footprint. This is what you need when working with Arduinos, ESPs and such.

# Installing the library
# Technical details
## How it works
## Adding your own sentences

It's quite simple to add your own NMEA sentence:
- declare its structure, with a default `bool isValid` and `int16_t fieldValidity` inside it.
- declare its type in `STRINGS_TYPES`
- create a parsing function based on existing ones
- declare your sentence type in the `dispatch` function.
- you're done!

## References

- [NMEA Sentences Reference](https://www.gpsinformation.org/dale/nmea.htm)

# Todo
- [ ] Add examples
- [ ] Complete unit tests
- [ ] Add way to compose sentences

# Copyright
## Logo
Icons made by <a href="https://www.flaticon.com/authors/freepik" title="Freepik">Freepik</a> from <a href="https://www.flaticon.com/" title="Flaticon">www.flaticon.com</a>