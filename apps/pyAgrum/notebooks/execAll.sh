for name in *.ipynb; do
  echo "- $name"
  jupyter nbconvert --ExecutePreprocessor.timeout=6000 --ExecutePreprocessor.kernel_name=devAgrum --execute --to notebook --inplace  $name
done
