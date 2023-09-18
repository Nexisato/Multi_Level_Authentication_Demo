#ifndef __G_H__
#define __G_H__

#include <iostream>
#include "Pairing.h"
#include "Zr.h"

using namespace std;

class G {//Group
public:

static void pow2(G &gout, const G &base1, const Zr &exp1, const G &base2, const Zr &exp2);
static void pow3(G &gout, const G &base1, const Zr &exp1, const G &base2, const Zr &exp2, const G &base3, const Zr &exp3);


  //Destructor
  ~G();

  bool isIdentity() const;
  void setElement(const element_t& el);

  //Create an element from hash
  //Assumes that g is already initialized
  //void setElement(const void* data,  unsigned short len);

  //Create an element from import
  //Assumes that g is already initialized and for element of type GT
  //bool compressed is not set to true
  //void setElement(const unsigned char *data, unsigned short len, 
  //				  bool compressed = false, unsigned short base = 16);

  const element_t& getElement() const;
  unsigned short getElementSize() const;
  bool isElementPresent() const{return elementPresent;}

  string toString() const;
  
  // Dump the element to stdout (print friendly)
  void dump(FILE *f, const char *label = NULL,
			unsigned short base = 16) const;	

protected:	
  element_t g;
  bool elementPresent;

  //Intialize with another element and assign identity or same element
  G(const G &h, bool identity=false);

  //Copy constructor
  //G(const G &h);

  //Create a null element
  G() {elementPresent = false;}

  //Create and initialize an element
  G(const Pairing &e){ 
	elementPresent = e.isPairingPresent();
  }

  // Assignment operator 
  G& operator=(const G &rhs);  

  //Arithmetic Assignment Operators
  G& operator*=(const G &rhs);
  G& operator/=(const G &rhs);
  G& operator^=(const Zr &exp);

  bool operator==(const G &rhs) const;
  const G inverse() const;
  const G square() const;

  void importElement(const unsigned char *data, unsigned short len,
      bool compressed, unsigned short base);

private:
  void nullify();

  friend ostream& operator<<(ostream &os, const G &g);
  friend istream& operator>>(istream &is, G &g);
};

// Preprocessing classes
template <class T>
class GPP {
public:
    GPP(const Pairing &pairing, const T &base):pairing(pairing) {
	element_pp_init(g_pp, *(element_t*)&base.getElement());
    }
    ~GPP() { element_pp_clear(g_pp); }

    const T operator^(const Zr &exp) const {
	T res(pairing);
	element_pp_pow_zn(*(element_t*)&res.getElement(),
			    *(element_t*)&exp.getElement(),
			    *(element_pp_t*)&g_pp);
	return res;
    }

private:
    element_pp_t g_pp;
    const Pairing &pairing;
};

#endif
