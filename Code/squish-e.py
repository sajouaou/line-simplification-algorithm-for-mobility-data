# -*- coding: utf-8 -*-

from copy import copy
from shapely.geometry import LineString, Point
import pandas as pd
from numpy import inf
from queue import PriorityQueue


import movingpandas as mpd

from mpd.trajectory_generalizer import *



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



class SquishE(OnlineTrajectoryGeneralizer):

    def reduce(self,Q,pred,succ,p):
        p_j = self.remove_min(Q)
        p[succ[p_j]] = max(priority(p_j),p[succ[p_j]])
        p[pred[p_j]] = max(priority(p_j),p[pred[p_j]])
        succ[pred[p_j]] = succ[p_j]
        pred[succ[p_j]] = pred[p_j]
        self.adjust_priority(succ[pred[p_j]],Q,pred,succ,p)
        self.adjust_priority(pred[pred[p_j]],Q,pred,succ,p)

        del p[p_j],pred[p_j],succ[p_j]
    def adjust_priority(self,point,Q,pred,succ,p):
        if pred[point] and succ[point]:
            priority = p[point] + SED(point,pred[point],succ[point])
            set_priority(point,priority,Q)

    def set_priority(self,point,priority,Q):
        Q.put((priority, point))

    def _generalize_traj(self, traj_stream, l,mu):
        ##Online part
        beta = 4
        i = 0
        Q = PriorityQueue()
        while traj_stream.open():
            p_i = traj_stream.fetch() # Get points from stream
            if i/l >= beta :
                beta += 1
            self.set_priority(p_i,inf,Q)
            p[p_i] = 0
            if i > 1 :
                succ[p_j] = p_i
                pred[p_i] = p_j
                self.adjust_priority(p_j,Q,pred,succ,p)
            if len(Q) = beta: # Q is full
                self.reduce(Q,pred,succ,p)
            p_j = p_i
            i += 1

        # Offline part
        priority = self.min_priority(Q)
        while priority <= mu :
            self.reduce(Q,pred,succ,p)
            priority = self.min_priority(Q)

        return traj

