#!/bin/sh
python ./1_gen_work_dir.py && ./2_run_preprocess.sh && python ./3_gen_digit_data_label.py;