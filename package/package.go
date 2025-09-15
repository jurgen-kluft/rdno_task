package rdno_task

import (
	denv "github.com/jurgen-kluft/ccode/denv"
	rdno_core "github.com/jurgen-kluft/rdno_core/package"
)

const (
	repo_path = "github.com\\jurgen-kluft"
	repo_name = "rdno_task"
)

func GetPackage() *denv.Package {
	name := repo_name

	// dependencies
	corepkg := rdno_core.GetPackage()

	// main package
	mainpkg := denv.NewPackage(repo_path, repo_name)
	mainpkg.AddPackage(corepkg)

	// main library
	mainlib := denv.SetupCppLibProject(mainpkg, name)
	mainlib.AddDependencies(corepkg.GetMainLib()...)

	// test library
	testlib := denv.SetupCppTestLibProject(mainpkg, name)
	testlib.AddDependencies(corepkg.GetTestLib()...)

	mainpkg.AddMainLib(mainlib)
	mainpkg.AddTestLib(testlib)
	return mainpkg
}
