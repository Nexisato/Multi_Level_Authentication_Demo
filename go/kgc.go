package main

import (
	"crypto/sha256"
	_ "crypto/sha256"
	_ "fmt"

	"github.com/Nik-U/pbc"
)

type KGC struct {
	params  *pbc.Params
	pairing *pbc.Pairing
	s       *pbc.Element

	P  *pbc.Element
	P0 *pbc.Element
}

func (kgc *KGC) Setup() {
	kgc.params = pbc.GenerateA(160, 512)
	kgc.pairing = kgc.params.NewPairing()
	kgc.s = kgc.pairing.NewZr().Rand()
	kgc.P = kgc.pairing.NewG1().Rand()
	kgc.P0 = kgc.pairing.NewG1().MulZn(kgc.P, kgc.s)
}

func (kgc *KGC) ShareParams() string {
	return kgc.params.String()
}

func (kgc *KGC) GeneratePartialKey(pid string) []byte {
	Q := kgc.pairing.NewG1().SetFromStringHash(pid, sha256.New())
	D := kgc.pairing.NewG1().MulZn(Q, kgc.s)
	return D.Bytes()
}
