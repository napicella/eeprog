package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

const numberOfWords = 8192

// main generates a 8192 word by 1 byte file. The destination file can be specifified
// with the -dest option. If the -desc option is specified the data byte starts from
// the max number representable with 8 bit (0xFF) and goes down to 0. Otherwise it
// starts from 0x00 and goes up to 0xFF. The data byte wraps around, i.e. after 0xFF
// the next byte written is 0x00. Similarly in -desc mode, after the 0x00, follows
// the 0xFF
func main() {
	path := flag.String("dest", "", "where to create the file, including the filename")
	desc := flag.Bool("desc", false, "starts from FF")
	help := flag.Bool("help", false, "prints the help message")
	flag.Parse()

	if *help {
		printHelp()
		return
	}

	if *path == "" {
		printHelp()
		log.Fatal("Error: -dest flag was not provided or it is empty")
	}
	f, e := os.Create(*path)
	if e != nil {
		log.Fatalf("Error: Unable to create file %s\n", *path)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	if !*desc {
		log.Println("Using increasing order")
		write(w, &increasingSupplier{
			current:       0,
			maxIterations: numberOfWords})
	} else {
		log.Println("Using decreasing order")
		write(w, &decreasingSupplier{
			current:       0xFF,
			maxIterations: numberOfWords})
	}
	w.Flush()
}

func write(w io.ByteWriter, source supplier) {
	for {
		if !source.hasNext() {
			break
		}
		w.WriteByte(byte(source.next()))
	}
}

type supplier interface {
	next() uint8
	hasNext() bool
}

type increasingSupplier struct {
	current       uint8
	iterations    int
	maxIterations int
}

func (t *increasingSupplier) next() uint8 {
	next := t.current
	t.iterations++
	t.current++
	return next
}

func (t *increasingSupplier) hasNext() bool {
	return t.iterations < t.maxIterations
}

type decreasingSupplier struct {
	current       uint8
	iterations    int
	maxIterations int
}

func (t *decreasingSupplier) next() uint8 {
	next := t.current
	t.iterations++
	t.current--
	return next
}

func (t *decreasingSupplier) hasNext() bool {
	return t.iterations < t.maxIterations
}

func printHelp() {
	fmt.Print(`
USAGE:	
The program generates a 8192 word by 1 byte file. The destination file can be specifified
with the -dest option. If the -desc option is specified the data byte starts from 
the max number representable with 8 bit (0xFF) and goes down to 0. Otherwise it
starts from 0x00 and goes up to 0xFF. The data byte wraps around, i.e. after 0xFF
the next byte written is 0x00. Similarly in -desc mode, after the 0x00, follows 
the 0xFF

-dest filename [-desc]
---

`)
}
