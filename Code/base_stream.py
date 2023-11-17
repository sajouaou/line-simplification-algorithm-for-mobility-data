


class Base_Stream:
    def is_open(self):
        return False

    def get_traj_id_col(self):
        return None

    def get_points(self):
        return None

class  Simulate_stream(Base_Stream):
    def __init__(self,traj):
        self.i = 0
        self.traj = traj.copy()
        self.id = self.traj.id
        self.crs = self.traj.crs

    def get_traj_id_col(self):
        return self.traj.get_traj_id_col()

    def is_open(self):
        return self.i < len(self.traj.df)

    def get_points(self):
        self.i += 1
        return self.traj.df.iloc[self.i - 1]
