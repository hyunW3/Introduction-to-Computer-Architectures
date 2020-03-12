make clean
make &&
mkdir result
#touch output_df.txt

./spim -f df.s | tee ./result/output_df.txt &&
./spim -f lw.s | tee ./result/output_lw_s.txt &&
./spim -f beq.s | tee ./result/output_beq.txt &&
./spim -f lw_b.s | tee ./result/output_lw_b.txt &&
./spim -f test_bp.s | tee ./result/output_test_bp.txt
