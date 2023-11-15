from tkinter import *
import numpy as np

class Window(Tk):
    def __init__(self):
        super().__init__()

        self.title("Hello World")

        self.generate = Button(text="Generate Path")
        self.button = Button(text="My simple app.")
        self.button.bind("<ButtonPress>", self.handle_button_press)
        self.generate.bind("<ButtonPress>", self.generate_path)

        self.canvas= Canvas(self, width=800, height=800, bg="white")
        self.canvas.pack()
        self.k = 200
        self.button.pack()
        self.generate.pack()

    def generate_path(self,event):
        print("Path generated")
        self.canvas.delete("all")
        self.path = [list(np.random.randint(800, size=(2))) for i in range(self.k)]
        self.canvas.create_line(self.path ,fill="gray", width=3 )


    def handle_button_press(self, event):
        self.destroy()


# Start the event loop.
window = Window()
window.mainloop()
