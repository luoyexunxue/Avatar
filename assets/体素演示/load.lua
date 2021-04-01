
scene.insert(nil,'skybox','skybox',{ texture = "px.jpg,nx.jpg,nz.jpg,pz.jpg,py.jpg,ny.jpg" })
scene.position('skybox',0,0,0)
scene.angle('skybox',0,0,0)
scene.scale('skybox',1,1,1)
scene.visible('skybox',true)
scene.material('skybox',-1,{roughness=0.8,metalness=0.1,color='#fff'})

scene.insert(nil,'geometry','center',{ shape = "box", texture = "logo.png" })
scene.position('center',0,0,0)
scene.angle('center',0,0,0)
scene.scale('center',1,1,1)
scene.visible('center',true)
scene.material('center',-1,{roughness=0.8,metalness=0.1,color='#fff'})

