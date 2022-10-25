To release a new version of force-delete-win:

1. git fetch upstream && git checkout upstream/main
2. git clean -xfdi
3. Update CHANGELOG.md with `gitchangelog`
4. git add -A && git commit -m "Update Changelog"
5. Update release version in Rust/Python ``Cargo.toml`` (set release version, remove 'dev')
6. Update version in main/Rust README
7. git add -A && git commit -m "Release vX.X.X"
8. git tag -a vX.X.X -m "Release vX.X.X"
9. Update development version in Rust/Python ``Cargo.toml`` (add '-dev' and increment minor version)
10. git add -A && git commit -m "Set development version to vY.Y.Y"
11. git push upstream main
12. git push upstream --tags
13. Create release in GitHub
14. Wait for GitHub actions to publish on crates.io and PyPi
