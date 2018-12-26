load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_github_nuxinl_argdata",
    commit = "686bffe0e2dacf39c4bfe08a5f0d8f30c3237d2f",
    remote = "https://github.com/NuxiNL/argdata.git",
)

git_repository(
    name = "com_github_nuxinl_bazel_third_party",
    commit = "1ea7fe04d7444391e388915ba24f1f9b204705ec",
    remote = "https://github.com/NuxiNL/bazel-third-party.git",
)

load("@com_github_nuxinl_bazel_third_party//:third_party.bzl", "third_party_repositories")

third_party_repositories()
