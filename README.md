# CHash — file hash modifier

CLI tool that modifies a file's hash by encrypting 
selected byte blocks using XOR with hardware-generated randomness.

**Stack:** C++

### How it works
- Selects a target file and byte position
- Generates random bytes using `std::random_device`
- XORs bytes with an obfuscation key and writes back to file

### Usage
1. Select file
2. Will modify last bytes of file

### Demo
<h4 align="center">Before</h4>
<p align="center">
  <img src="https://github.com/Roflane/CHash/blob/main/before.png" />
</p>
<h4 align="center">After</h4>
<p align="center">
  <img src="https://github.com/Roflane/CHash/blob/main/after.png" />
</p>
