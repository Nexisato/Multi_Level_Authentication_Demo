package main

import (
	"crypto/sha256"

	"github.com/Nik-U/pbc"
)

type Payload struct {
	Message   string
	Signature [2][]byte
}

type Process struct {
	pairing      *pbc.Pairing
	secret_value *pbc.Element
	secret_key   *pbc.Element

	PubKey [2]*pbc.Element
	Pid    string
}

func (p *Process) Setup(params string, pid string) {
	p.Pid = pid
	p.pairing, _ = pbc.NewPairingFromString(params)
	p.secret_value = p.pairing.NewZr().Rand()
}

func (p *Process) GenerateFullKey(partial_key []byte, P_bytes []byte, P0_bytes []byte) {
	D := p.pairing.NewG1().SetBytes(partial_key)
	P := p.pairing.NewG1().SetBytes(P_bytes)
	P0 := p.pairing.NewG1().SetBytes(P0_bytes)

	p.secret_key = p.pairing.NewG1().MulZn(D, p.secret_value)

	p.PubKey[0] = p.pairing.NewG1().MulZn(P, p.secret_value)
	p.PubKey[1] = p.pairing.NewG1().MulZn(P0, p.secret_value)
}

func (p *Process) SignMessage(message string, P_bytes []byte) (Payload, [2][]byte) {
	ri := p.pairing.NewZr().Rand()
	P := p.pairing.NewG1().SetBytes(P_bytes)

	ri_P := p.pairing.NewG1().MulZn(P, ri)
	T := p.pairing.NewGT().Pair(P, ri_P)

	T_str := string(T.Bytes())

	msg_input := message + T_str

	V := p.pairing.NewZr().SetFromStringHash(msg_input, sha256.New())

	U := p.pairing.NewG1().MulZn(p.secret_key, V)
	U = U.Add(U, ri_P)

	payload := Payload{
		Message: message,
		Signature: [2][]byte{
			U.Bytes(),
			V.Bytes(),
		},
	}

	pubkey_bytes := [2][]byte{
		p.PubKey[0].Bytes(),
		p.PubKey[1].Bytes(),
	}

	return payload, pubkey_bytes

}

func (p *Process) VerifyMessage(pid string, payload Payload, params string,
	P_bytes []byte, P0_bytes []byte, pubkey [2][]byte) bool {
	// Extract Params
	pairing, _ := pbc.NewPairingFromString(params)

	P := pairing.NewG1().SetBytes(P_bytes)
	P0 := pairing.NewG1().SetBytes(P0_bytes)
	PubKey := [2]*pbc.Element{
		pairing.NewG1().SetBytes(pubkey[0]),
		pairing.NewG1().SetBytes(pubkey[1]),
	}

	U := pairing.NewG1().SetBytes(payload.Signature[0])
	V := pairing.NewZr().SetBytes(payload.Signature[1])

	// Verify PubKey
	lhs := pairing.NewGT().Pair(PubKey[0], P0)
	rhs := pairing.NewGT().Pair(PubKey[1], P)
	if !lhs.Equals(rhs) {
		return false
	}

	// Verify Sinature
	Q := pairing.NewG1().SetFromStringHash(pid, sha256.New())
	pair1 := pairing.NewGT().Pair(U, P)

	tmpY := pairing.NewG1()
	tmpY = tmpY.Neg(PubKey[1])
	tmpY = tmpY.MulZn(tmpY, V)

	pair2 := pairing.NewGT().Pair(Q, tmpY)

	T := pairing.NewGT().Mul(pair1, pair2)

	T_str := string(T.Bytes())
	msg_input := payload.Message + T_str
	//fmt.Println("msg_input: ", msg_input)
	msg_hash := pairing.NewZr().SetFromStringHash(msg_input, sha256.New())

	if !msg_hash.Equals(V) {
		return false
	}
	return true
}
