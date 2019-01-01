load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "org_cloudabi_argdata",
    commit = "686bffe0e2dacf39c4bfe08a5f0d8f30c3237d2f",
    remote = "https://github.com/NuxiNL/argdata.git",
)

git_repository(
    name = "org_cloudabi_bazel_third_party",
    commit = "8d51abda2299d5fe26ca7c55f182b6562f440979",
    remote = "https://github.com/NuxiNL/bazel-third-party.git",
)

load("@org_cloudabi_bazel_third_party//:third_party.bzl", "third_party_repositories")

third_party_repositories()
