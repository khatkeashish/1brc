package main

import (
	"bufio"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
)

type CityResult struct {
	Mean    float64
	Min     float64
	Max     float64
	Counter int
	Sum     float64
}

func NewCityResult() *CityResult {
	return &CityResult{
		Mean:    0.0,
		Min:     math.Inf(1),
		Max:     math.Inf(-1),
		Counter: 0,
		Sum:     0.0,
	}
}

func main() {
	file, err := os.Open("test_sample.txt")
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	// Create a new scanner
	scanner := bufio.NewScanner(file)

	results := make(map[string]*CityResult)
	// Read line by line
	for scanner.Scan() {
		city, valStr, _ := strings.Cut(scanner.Text(), ";")
		// Get the value for a given key
		cResult, exists := results[city]
		if !exists {
			cResult = NewCityResult()
			results[city] = cResult
		}
		val, _ := strconv.ParseFloat(valStr, 64)
		cResult.Counter++
		cResult.Min = math.Min(cResult.Min, val)
		cResult.Max = math.Max(cResult.Max, val)
		cResult.Sum += val
	}

	var builder strings.Builder

	for city, cr := range results {
		cr.Mean = cr.Sum / float64(cr.Counter)
		line := fmt.Sprintf("%s;%.6f;%.6f;%.6f\n", city, cr.Mean, cr.Min, cr.Max)
		builder.WriteString(line)
	}

	err = os.WriteFile("test_sample_results_calculated.txt", []byte(builder.String()), 0644)
	if err != nil {
		panic(err)
	}

}
