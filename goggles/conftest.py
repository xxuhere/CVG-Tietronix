import pytest


def pytest_addoption(parser):
    parser.addoption(
        "--runfpga", action="store_true", default=False, help="run fpga tests"
    )


def pytest_configure(config):
    config.addinivalue_line("markers", "fpga: mark test as requiring fpga to run")


def pytest_collection_modifyitems(config, items):
    if config.getoption("--runfpga"):
        return
    skip_fpga = pytest.mark.skip(reason="need --runfpga option to run")
    for item in items:
        if "fpga" in item.keywords:
            item.add_marker(skip_fpga)
