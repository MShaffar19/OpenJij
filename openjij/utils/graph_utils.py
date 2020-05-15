import numpy as np

def qubo_to_ising(mat: np.ndarray):
    """inplace-convert numpy matrix from qubo to ising.

    Args:
        mat (np.ndarray): numpy matrix

    """
    mat /= 4
    for i in range(len(mat)):
        mat[i, i] += np.sum(mat[i, :])
    

def chimera_to_ind(r: int, c: int, z: int, L: int):
    """[summary]

    Args:
        r (int): [description]
        c (int): [description]
        z (int): [description]
        L (int): [description]

    Raises:
        ValueError: [description]

    Returns:
        [int]: [description]
    """
    if not (0 <= r < L and 0 <= c < L and 0 <= z < 8):
        raise ValueError(
            '0 <= r < L or 0 <= c < L or 0 <= z < 8. '
            'your input r={}, c={}, z={}, L={}'.format(r, c, z, L))
    return r * L * 8 + c*8 + z
