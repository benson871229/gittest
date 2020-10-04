from flask import Flask, render_template, send_file,request
import matplotlib.pyplot as plt
from io import BytesIO
import networkx as nx
import sqlite3
import pandas as pd
import csv
app = Flask(__name__)

@app.route('/',methods=['GET', 'POST'])
def login():
    #  利用request取得使用者端傳來的方法為何
    if request.method == 'POST':
                          #  利用request取得表單欄位值
        return  render_template('search.html')
    
    #  非POST的時候就會回傳一個空白的模板
    return render_template('home.html')

@app.route('/graph',methods=['GET', 'POST'])

def graph(i=0): 
    node1=pd.read_csv('file.csv',sep='\s+',skiprows=i)
    if node1.empty:    
        G = nx.star_graph(node1)
        ip = request.form['searchbox'] 
        G.add_nodes_from(node1)
        
        f=nx.compose(G,G)

        nx.draw(f,with_labels=True)
        plt.show                    
        img = BytesIO() # file-like object for the image
        plt.savefig(img) # save the image to the stream
        img.seek(0) # writing moved the cursor to the end of the file, reset
        plt.clf() # clear pyplot
        return send_file(img, mimetype='image/png')
    else:
        G = nx.Graph()
        ip = request.form['searchbox'] 
        G.add_nodes_from(node1)
        G = nx.star_graph(node1)
        nx.draw(G,with_labels=True)
        plt.show            
        

        return graph(i+1)
             
    
    

if __name__ == '__main__':
    app.run(debug=True)