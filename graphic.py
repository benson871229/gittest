from flask import Flask, render_template, send_file,request
import matplotlib.pyplot as plt
from io import BytesIO
import networkx as nx
import sqlite3
import pandas as pd
import csv
import numpy as np
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

def graph(): 
    node=pd.read_csv('file.csv',sep='\s+')

    ip = request.form['searchbox'] 
    node0=pd.read_csv('file.csv',sep='\s+',skiprows=0)
   

    G=nx.star_graph(node0)
  

    for i in range(0,len(node)+1):
        node1=pd.read_csv('file.csv',sep='\s+',skiprows=i)
        G1=nx.star_graph(node1)       
        f=nx.compose(G,G1)
        G=f
     
       

    
    
    nx.draw(f,with_labels=True)
    
     
    
    plt.show                    
    img = BytesIO() # file-like object for the image
    plt.savefig(img) # save the image to the stream
    img.seek(0) # writing moved the cursor to the end of the file, reset
    plt.clf() # clear pyplot
    return send_file(img, mimetype='image/png')
 
        
        
        
      
        

  
        
        
        
      
        

  
             
    
    

if __name__ == '__main__':
    app.run(debug=True)