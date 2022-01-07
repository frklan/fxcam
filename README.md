# fxcam

Playing with face detection using openCV and GUI stuff using SSDL2 and ImGUI. Mostly useless but a great learning adventure.

## Dependencies

Dependencies are handled either by conan or cmake FetchContent and should install automatically when configuring the build.

The following dependencies are used:
- spdlog/1.9.2
- sdl/2.0.18
- opencv/4.5.3
- imgui/1.85

Models for the face detector comes from https://github.com/spmallick/learnopencv/tree/master/FaceDetectionComparison/models

## Compiling

To configure and build, do the following

```
git clone ...
cd fxcam
take build
cmake .. --preset default
make -j
```

The built executable should end up in src/.

## Known issues

- Some (?) all of the conan dependencies are compiled with -fvisibility=hidden, I current get **a lot** of linker warnings due to this. 
- clang-tidy and cppcheck spews out hundreds of warnings on the dependencies handled by cmake (via FetchContent) and the ImGui-to-SDL bindings in externals/. I've silenced clang-tidy as much as possible by disabling most checks globally..

## Other resources

This code uses the docking branch of ImGUI to make windows "dock-able". A neat example of this is available [here](https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2)


## Contributing

Contributions are always welcome!

When contributing to this repository, please first discuss the change you wish to make via the issue tracker, email, or any other method with the owner of this repository before making a change.

Please note that we have a code of conduct, you are required to follow it in all your interactions with the project.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/frklan/[TBD]/tags).

## Authors

* **Fredrik Andersson** - [frklan](https://github.com/frklan)

## License

This project is licensed under the CC BY-NC-SA License - see the [LICENSE](LICENSE) file for details

For commercial/proprietary licensing, please contact the project owner

App icon from [icon-icons](https://icon-icons.com)


