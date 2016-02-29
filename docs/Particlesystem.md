# Particle system module

## Spawn module 

spawn { 
	texture : 40,160,20,20
    duration : 0
    duration_variance : 0
    rate : 100 
    loop : 0
    loop_delay : 1.0
}

## Ring location

ring_location {
    radius : 30
    variance : 4
    angle_variance : 180
    step : 0
}

## Lifecycly module

lifecycle { 
    ttl : 1.5
    variance : 0.2
}

## Linear color module

linear_color { 
    start_color : 246,200,138,255
    end_color : 246,200,138,0
}

## Size module

size { 
	initial : 1.0,1.0
	variance : 0.2,0.2
}

size { 
	initial : 1.0,1.0
	variance : 0.2,0.2
    min : 0.3,0.3
    max : 0.8,0.3
}

size { 
	initial : 1.0,1.0
	variance : 0.2,0.2
    path : 0.0,1.0,1.0,0.4,0.8,0.8,0.8,1.4,1.4,1.0,1.0,1.0
}

## Radial velocity module 

radial_velocity {
	velocity : 40
	variance : 20
}

## Damping velocity module

damping_velocity {
	damping : 140
}

## Color module

color {
	color : 255,0,255,255
}

color {
	hsv : 34,44,96
	alpha : 255
	hue_variance : 10
	saturation_variance : 10
	value_variance : 10
}

## Alpha module

alpha {
	initial : 1.0
	variance : 0.2
}

alpha {
	initial : 1.0
	variance : 0.2
	min : 1.0
	max : 0.1
}

alpha {
	initial : 1.0
	variance : 0.2
	path : 0.0,1.0,0.4,0.8,0.8,0.2,1.0,0.0
}

## Rotation

rotation {
	velocity : 360
	variance : 720
}

## Velocity 

velocity {
	velocity : 200,0
	variance : 100,0
	distribution : 0.0,1.0,1.0,0.5,-0.5,-0.5,1.0,0.0,0.0
}

velocity {
	radial : 140
	radialVariance : 20
}

velocity {
	radial : 140
	radialVariance : 20
	distribution : 0.0,1.0,1.0,0.5,-0.5,-0.5,1.0,0.0,0.0
}