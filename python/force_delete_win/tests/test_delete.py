
import os
import os.path as osp
import subprocess

from force_delete_win import force_delete_file_folder


def test_delete(tmp_path):
    tmp_dir = tmp_path / "test_dir"
    tmp_dir.mkdir()
    loc = str(tmp_dir)
    subprocess.Popen(["cmd.exe", "/C", "start", "cmd"], cwd=loc,
                     stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                     stderr=subprocess.PIPE)

    # Ensure that the folder cannot be deleted
    try:
        os.removedirs(loc)
        assert False
    except Exception:
        pass

    assert force_delete_file_folder(loc)
    assert not osp.exists(loc)
