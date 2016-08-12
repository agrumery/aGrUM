for name in *.ipynb; do
  echo "- $name"
  jupyter nbconvert --ExecutePreprocessor.timeout=6000 --execute --to notebook --output $name  $name
done
