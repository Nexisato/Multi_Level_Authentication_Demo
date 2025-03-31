# # MLAuth(Python Version)
> Ineteraction proecess supported Only

## Requirements
- Python > 3.8


## Usage
```bash
pip install -r requirements.txt
```
If you need the wrapper of Python bingding version of PBC library, 
the [modified version of pypbc](https://github.com/Jemtaly/pypbc/tree/master) is recommended(Type A curve supported only). 

- Follow the instructions bellow 👇

```bash
git clone https://github.com/Jemtaly/pypbc/tree/master
sudo apt update 
sudo apt install libgmp-dev build-essential flex bison

wget https://crypto.stanford.edu/pbc/files/pbc-0.5.14.tar.gz
tar -xf pbc-0.5.14.tar.gz
cd pbc-0.5.14
./configure --prefix=/usr --enable-shared
make -j
sudo make install
sudo ldconfig
pip install .
```




## References
- https://github.com/Jemtaly/pypbc


