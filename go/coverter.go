package main

import "fmt"

func main() {
	pid1 := "0x00000001"
	pid2 := "0x00000002"

	kgc1 := &KGC{}
	kgc1.Setup()

	params := kgc1.ShareParams()
	fmt.Println(params)

	D1_bytes := kgc1.GeneratePartialKey(pid1)

	process1 := &Process{}
	process1.Setup(params, pid1)
	process1.GenerateFullKey(D1_bytes, kgc1.P.Bytes(), kgc1.P0.Bytes())

	msg := "Hello World!"
	payload, pubkey1_bytes := process1.SignMessage(msg, kgc1.P.Bytes())

	fmt.Println("payload: ", payload.Message)
	// kgc2, Process2
	kgc2 := &KGC{}
	kgc2.Setup()
	params2 := kgc2.ShareParams()
	process2 := &Process{}
	process2.Setup(params2, pid2)

	var res bool
	res = process2.VerifyMessage(pid1, payload, params,
		kgc1.P.Bytes(), kgc1.P0.Bytes(), pubkey1_bytes)
	if res {
		fmt.Println("[Verify] Success")
	} else {
		fmt.Println("[Verify] Fail")
	}
}
