# Policies

- Always branch when adding a feature / fixing bugs.
- No need to branch when modifying documentation files e.g. README.md.
- Atleast one approval in order to be allowed to merge to main.
- Use unittests when possible.
- TODO: Nasa.
- Use camelCase.
- Newline in-between functions / scopes.
- TODO: Name branch after ticket name.

# Building

```
./scripts/build.sh
```
or build manually
```
cmake -B build
cmake --build build
```

# Testing

```
./scripts/run_tests.sh
```
or build manually
```
cmake -B build -DPICO_BUILD=OFF -DPICO_TESTS_BUILD=ON
cmake --build build
ctest --output-on-failure
```
# Raspberry Pi Pico W setup

```
<img width="1056" height="808" alt="image" src="https://github.com/user-attachments/assets/f034b3f5-f191-4720-bd3b-3e5743d7748a" />

Note: Although not visible in the photo, the buttons are wired to GPIO6, GPIO7, and GPIO8 respectively, starting from the top.
```
