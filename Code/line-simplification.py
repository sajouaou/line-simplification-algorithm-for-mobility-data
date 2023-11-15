from tkinter import *
import numpy as np
from rdp import rdp
import visvalingamwyatt as vw
from shapely import *
from time import sleep

class Window(Tk):
    def __init__(self):
        super().__init__()

        self.title("Hello World")

        self.generate = Button(text="Generate Path")
        self.simplify = Button(text="Simplify Path")
        self.button = Button(text="My simple app.")

        self.button.bind("<ButtonPress>", self.handle_button_press)
        slider = Scale(self, from_=0.1, to=25.0, orient="horizontal", length=250,digits=3,resolution=0.1, command=self.update_epsilon)
        slider.pack()
        self.generate.bind("<ButtonPress>", self.generate_path)
        self.simplify.bind("<ButtonPress>", self.simplify_path)
        self.canvas= Canvas(self, width=800, height=800, bg="white")
        self.canvas.bind("<ButtonPress>", self.add_point)

        self.path = []
        self.canvas.pack()
        self.k = 200
        self.epsilon = 0.01
        self.button.pack()
        self.generate.pack()
        self.simplify.pack()
    def update_epsilon(self,value):
        self.epsilon = float(value)
        self.simplify_path(None)
    def add_point(self,event):
        x = event.widget.canvasx(event.x)
        y = event.widget.canvasy(event.y)
        self.path = self.path + [(x,y)]
        self.canvas.delete("all")
        self.canvas.create_line(self.path ,fill="gray", width=3 )

    def simplify_path(self,event):
        print("Path Simplified")
        self.simple = rdp(self.path, self.epsilon)
        #self.simple = vw.simplify(self.path,threshold=self.epsilon)
        self.canvas.delete("all")
        self.canvas.create_line(self.path ,fill="gray", width=3 )
        self.canvas.create_line(self.simple ,fill="red", width=3 )
        print( "Frechet distance : ",frechet_distance(LineString(self.path), LineString(self.simple)))
        print( "Hausdorff distance : " ,hausdorff_distance(LineString(self.path), LineString(self.simple)))

    def generate_path(self,event):
        print("Path generated")
        self.canvas.delete("all")
        #self.path = [list(np.random.randint(800, size=(2))) for i in range(self.k)]
        self.path = [(38.0, 670.0), (53.0, 595.0), (58.0, 559.0), (66.0, 514.0), (73.0, 464.0), (77.0, 431.0), (77.0, 405.0), (72.0, 374.0), (69.0, 338.0), (66.0, 311.0), (65.0, 287.0), (76.0, 242.0), (89.0, 210.0), (105.0, 183.0), (144.0, 162.0), (179.0, 158.0), (198.0, 176.0), (213.0, 202.0), (229.0, 232.0), (246.0, 279.0), (254.0, 310.0), (263.0, 378.0), (268.0, 429.0), (272.0, 492.0), (280.0, 537.0), (294.0, 591.0), (312.0, 617.0), (356.0, 628.0), (391.0, 616.0), (422.0, 577.0), (439.0, 547.0), (460.0, 500.0), (475.0, 432.0), (480.0, 381.0), (479.0, 344.0), (480.0, 295.0), (481.0, 259.0), (486.0, 231.0), (497.0, 210.0), (507.0, 195.0), (522.0, 176.0), (546.0, 162.0), (577.0, 152.0), (606.0, 147.0), (637.0, 151.0), (667.0, 166.0), (686.0, 189.0), (699.0, 224.0), (707.0, 266.0), (712.0, 306.0), (714.0, 348.0), (715.0, 377.0), (707.0, 404.0), (694.0, 438.0), (684.0, 460.0), (669.0, 483.0), (659.0, 515.0), (658.0, 543.0), (668.0, 562.0), (695.0, 590.0), (724.0, 602.0), (771.0, 605.0), (794.0, 609.0), (790.0, 651.0)]
        i = 2
        step = 1
        while i <= len(self.path) :
            self.canvas.create_line(self.path[:i] ,fill="gray", width=3 )
            self.canvas.update()
            i += step
            sleep(0.1)

        self.canvas.delete("all")
        self.canvas.create_line(self.path,fill="gray", width=3 )

    def handle_button_press(self, event):
        self.destroy()


# Start the event loop.
window = Window()
window.mainloop()
