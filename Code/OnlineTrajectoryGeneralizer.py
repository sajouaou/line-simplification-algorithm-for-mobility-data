# -*- coding: utf-8 -*-

from copy import copy
from shapely.geometry import LineString, Point
import pandas as pd
import numpy as np

from geopandas import GeoDataFrame
import movingpandas as mpd

from movingpandas.trajectory import Trajectory




class OnlineTrajectoryGeneralizer:

    """
    Online Generalizer base class
    """

    def __init__(self, traj_stream):
        """
        Create TrajectoryGeneralizer

        Parameters
        ----------
        traj_stream : Trajectory stream (Only 1 trajectory stream)
        """
        self.traj_stream = traj_stream


    def generalize(self, tolerance,epsilon):
        """
        Generalize the input Trajectory/TrajectoryCollection.

        Parameters
        ----------
        tolerance : any type
            Tolerance threshold, differs by generalizer

        Returns
        -------
        Trajectory/TrajectoryCollection
            Generalized Trajectory or TrajectoryCollection
        """
        return self._generalize_traj(self.traj_stream, tolerance,epsilon)

    def _generalize_traj(self, traj_stream, tolerance,epsilon):
        return traj_stream


def SED(A,B,C):
    x_A =  A['geometry'].x; y_A = A['geometry'].y; t_A = A.name
    x_B =  B['geometry'].x; y_B = B['geometry'].y; t_B = B.name
    x_C =  C['geometry'].x; y_C = C['geometry'].y; t_C = C.name

    v_x_AC = (x_C - x_A ) / (t_C - t_A).total_seconds()
    v_y_AC = (y_C - y_A ) / (t_C - t_A).total_seconds()

    xp_B = x_A + v_x_AC * (t_B - t_A).total_seconds()
    yp_B = y_A + v_y_AC * (t_B - t_A).total_seconds()
    return np.sqrt((xp_B - x_B )**2  + ( yp_B  - y_B)**2 )



class SquishE(OnlineTrajectoryGeneralizer):

    def reduce(self,Q,pred,succ,p):
        priority,p_j = self.remove_min(Q)
        p[str(succ[str(p_j)])] = max(priority,p[str(succ[str(p_j)])])
        p[str(pred[str(p_j)])] = max(priority,p[str(pred[str(p_j)])])
        succ[str(pred[str(p_j)])] = succ[str(p_j)]
        pred[str(succ[str(p_j)])] = pred[str(p_j)]
        self.adjust_priority(succ[str(p_j)],Q,pred,succ,p)
        self.adjust_priority(pred[str(p_j)],Q,pred,succ,p)

        del p[str(p_j)],pred[str(p_j)],succ[str(p_j)]

    def adjust_priority(self,point,Q,pred,succ,p):
        if pred.get(str(point)) is not None  and succ.get(str(point)) is not None:
            priority = p[str(point)] + SED(pred[str(point)],point,succ[str(point)])
            self.set_priority(point,priority,Q)


    def remove_min(self,Q):
        tuple = Q[0].pop(-1)
        return tuple[0],Q[1].pop(tuple[1])

    def min_priority(self,Q):
        return Q[0][-1][0]

    def set_priority(self,point,priority,Q):
        index = [i for i in range(len(Q[0])) if Q[0][i][1] == point["id"]]
        if len(index) == 0:
            Q[0].append([priority, point["id"]])
            Q[1][point["id"]] = point
        else :
            Q[0][index[0] ][0] = priority
        Q[0].sort(reverse=True)

    def _generalize_traj(self, traj_stream, l,mu):
        ##Online part
        beta = 4
        p = {}
        succ = {}
        pred = {}
        i = 0
        Q = [[],{}]
        while traj_stream.is_open():
            # Get points from stream
            p_i = traj_stream.get_points()
            if i == 0 :
                point = p_i
            if i/l >= beta :
                beta += 1
            self.set_priority(p_i,np.inf,Q)
            p[str(p_i)] = 0
            if i >= 1 :
                succ[str(p_j)] = p_i
                pred[str(p_i)] = p_j
                self.adjust_priority(p_j,Q,pred,succ,p)
            if len(Q[0]) == beta: # Q is full
                self.reduce(Q,pred,succ,p)
            p_j = p_i
            i += 1

        # Offline part
        #priority = self.min_priority(Q)
        #while priority <= mu :
        #    self.reduce(Q,pred,succ,p)
        #    priority = self.min_priority(Q)

        generalized = GeoDataFrame(pd.DataFrame([point]))
        while point is not None and succ.get(str(point)) is not None:
            point = succ.get(str(point))
            generalized = pd.concat( [ generalized ,GeoDataFrame(pd.DataFrame([point]) )] )
        generalized.index.name = "t"
        generalized.crs = self.traj_stream.crs


        return Trajectory( generalized, self.traj_stream.id, traj_id_col=self.traj_stream.get_traj_id_col())
