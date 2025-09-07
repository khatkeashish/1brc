create_test_samples:
	python create_test_samples.py 

run_python:
	time python solution_py.py
	python evaluate_test.py 
	rm test_sample_results_calculated.txt

