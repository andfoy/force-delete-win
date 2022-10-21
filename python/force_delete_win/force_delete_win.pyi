# -*- coding: utf-8 -*-

"""Stub typing declarations for force-delete-win."""

def force_delete_file_folder(path: str) -> bool:
    """
    Force-delete a file or folder that is being held by other processes.

    Parameters
    ----------
    fname: str
        Full path to the file or folder to force-delete.

    Returns
    -------
    `True` if the call was successful, else an error will be returned.

    Notes
    -----
    This function will close all the handles of all the processes that have
    opened the requested file or directory, thus it may cause unexpected
    behaviour on other programs or could leave your file system on an
    inconsistent state. USE THIS UNDER YOUR OWN RISK.
    """
    ...
