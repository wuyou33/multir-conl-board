/*
 * controllersTask.c
 *
 * Created: 13.9.2014 23:57:14
 *  Author: Tomas Baca
 */ 

#include "controllersTask.h"
#include "controllers.h"
#include "system.h"
#include "config.h"
#include "kalman.h"
#include "matrixLib.h"

float dt = 0.015;
#define NUMBER_OF_STATES 3
#define NUMBER_OF_INPUTS 1
#define NUMBER_OF_MEASURED_STATES 1

void controllersTask(void *p) {
	
	usartBufferPutString(usart_buffer_4, "---------------------------------\n\r", 10);

	kalmanHandler elevatorHandler;
	
	/* -------------------------------------------------------------------- */
	/* System A matrix														*/
	/* -------------------------------------------------------------------- */
	matrix_float A_matrix;
	A_matrix.name = "System matrix A";
	A_matrix.height = NUMBER_OF_STATES;
	A_matrix.width = NUMBER_OF_STATES;
	float data_A[NUMBER_OF_STATES*NUMBER_OF_STATES] = {1, dt, 0,
													   0, 1, 2.4239*dt,
													   0, 0, 1};
	A_matrix.data = (float *) data_A;
	
	/* -------------------------------------------------------------------- */
	/* System B matrix														*/
	/* -------------------------------------------------------------------- */
	matrix_float B_matrix;
	B_matrix.name = "System matrix B";
	B_matrix.height = NUMBER_OF_STATES;
	B_matrix.width = NUMBER_OF_INPUTS;
	float data_B[NUMBER_OF_STATES*NUMBER_OF_INPUTS] = {0, 0, 0.1219*dt};
	B_matrix.data = (float *) data_B;
	
	/* -------------------------------------------------------------------- */
	/* Kalman states vector													*/
	/* -------------------------------------------------------------------- */
	vector_float states_vector;
	elevatorHandler.states = &states_vector;
	states_vector.name = "Kalman states vector";
	states_vector.length = NUMBER_OF_STATES;
	states_vector.orientation = 0;
	float data_states[NUMBER_OF_STATES];
	states_vector.data = (float *) data_states;
	
	/* -------------------------------------------------------------------- */
	/* Kalman covariance matrix												*/
	/* -------------------------------------------------------------------- */
	matrix_float covariance_matrix;
	elevatorHandler.covariance = &covariance_matrix;
	covariance_matrix.name = "Kalman covariance matrix";
	covariance_matrix.height = NUMBER_OF_STATES;
	covariance_matrix.width = NUMBER_OF_STATES;
	float data_covariance[NUMBER_OF_STATES*NUMBER_OF_STATES];
	covariance_matrix.data = (float *) data_covariance;
	
	/* -------------------------------------------------------------------- */
	/* Measurement vector													*/
	/* -------------------------------------------------------------------- */
	vector_float measurement_vector;
	measurement_vector.name = "Measurements vector";
	measurement_vector.length = NUMBER_OF_MEASURED_STATES;
	measurement_vector.orientation = 0;
	float data_measurement[NUMBER_OF_MEASURED_STATES];
	measurement_vector.data = (float *) data_measurement;
	
	/* -------------------------------------------------------------------- */
	/* Process noise matrix	(R)												*/
	/* -------------------------------------------------------------------- */
	matrix_float R_matrix;
	R_matrix.name = "R matrix";
	R_matrix.width = NUMBER_OF_STATES;
	R_matrix.height = NUMBER_OF_STATES;
	float R_data[NUMBER_OF_STATES*NUMBER_OF_STATES] = {1, 0, 0,
													   0, 1, 0,
													   0, 0, 5};
	R_matrix.data = (float *) R_data;
	
	/* -------------------------------------------------------------------- */
	/* Measurement noise matrix	(Q)											*/
	/* -------------------------------------------------------------------- */
	matrix_float Q_matrix;
	Q_matrix.name = "Q matrix";
	Q_matrix.height = NUMBER_OF_MEASURED_STATES;
	Q_matrix.width = NUMBER_OF_MEASURED_STATES;
	float Q_data[NUMBER_OF_MEASURED_STATES*NUMBER_OF_MEASURED_STATES] = {1000};
	Q_matrix.data = (float *) Q_data;
	
	/* -------------------------------------------------------------------- */
	/* C matrix (transfare measurements -> states)							*/
	/* -------------------------------------------------------------------- */
	matrix_float C_matrix;
	C_matrix.name = "C matrix";
	C_matrix.height = NUMBER_OF_MEASURED_STATES;
	C_matrix.width = NUMBER_OF_STATES;
	float data_C[NUMBER_OF_MEASURED_STATES*NUMBER_OF_STATES] = {0, 1, 0};
	C_matrix.data = (float *) data_C;
	
	/* -------------------------------------------------------------------- */
	/* Aux matrices															*/
	/* -------------------------------------------------------------------- */
	
	// temp vector 
	vector_float temp_vector_n;
	elevatorHandler.temp_vector_n = &temp_vector_n;
	float temp_vector_data[NUMBER_OF_STATES];
	elevatorHandler.temp_vector_n->name = "temp_vector";
	elevatorHandler.temp_vector_n->data = (float *) &temp_vector_data;
	elevatorHandler.temp_vector_n->length = NUMBER_OF_STATES;
	elevatorHandler.temp_vector_n->orientation = 0;
	
	// temp matrix
	matrix_float temp_matrix_n_n;
	elevatorHandler.temp_matrix_n_n = &temp_matrix_n_n;
	float temp_matrix_data[NUMBER_OF_STATES*NUMBER_OF_STATES];
	elevatorHandler.temp_matrix_n_n->name = "temp_matrix";
	elevatorHandler.temp_matrix_n_n->data = (float *) &temp_matrix_data;
	elevatorHandler.temp_matrix_n_n->height = NUMBER_OF_STATES;
	elevatorHandler.temp_matrix_n_n->width = NUMBER_OF_STATES;
	
	// temp matrix2 
	matrix_float temp_matrix2_n_n;
	elevatorHandler.temp_matrix2_n_n = &temp_matrix2_n_n;
	float temp_matrix2_data[NUMBER_OF_STATES*NUMBER_OF_STATES];
	elevatorHandler.temp_matrix2_n_n->name = "temp_matrix2";
	elevatorHandler.temp_matrix2_n_n->data = (float *) &temp_matrix2_data;
	elevatorHandler.temp_matrix2_n_n->height = NUMBER_OF_STATES;
	elevatorHandler.temp_matrix2_n_n->width = NUMBER_OF_STATES;
	
	// temp matrix for computing the kalman gain
	matrix_float temp_matrix3_u_n;
	elevatorHandler.temp_matrix3_u_n = &temp_matrix3_u_n;
	float temp_matrix3_data[NUMBER_OF_INPUTS*NUMBER_OF_STATES];
	elevatorHandler.temp_matrix3_u_n->name = "temp_matrix3";
	elevatorHandler.temp_matrix3_u_n->data = (float *) &temp_matrix3_data;
	elevatorHandler.temp_matrix3_u_n->height = NUMBER_OF_INPUTS;
	elevatorHandler.temp_matrix3_u_n->width = NUMBER_OF_STATES;
	
	matrix_float temp_matrix4_u_u;
	float temp_matrix4_data[NUMBER_OF_INPUTS*NUMBER_OF_INPUTS];
	elevatorHandler.temp_matrix4_u_u = &temp_matrix4_u_u;
	elevatorHandler.temp_matrix4_u_u->name = "temp_matrix4";
	elevatorHandler.temp_matrix4_u_u->data = (float *) &temp_matrix4_data;
	elevatorHandler.temp_matrix4_u_u->height = NUMBER_OF_INPUTS;
	elevatorHandler.temp_matrix4_u_u->width = NUMBER_OF_INPUTS;
	
	// temp vector2
	vector_float temp_vector2;
	elevatorHandler.temp_vector_u = &temp_vector2;
	float temp_vector2_data[NUMBER_OF_INPUTS];
	elevatorHandler.temp_vector_u->data = (float *) &temp_vector2_data;
	elevatorHandler.temp_vector_u->length = NUMBER_OF_INPUTS;
	elevatorHandler.temp_vector_u->orientation = 0;
	
	elevatorHandler.number_of_states = NUMBER_OF_STATES;
	elevatorHandler.number_of_inputs = NUMBER_OF_INPUTS;
	
	/* -------------------------------------------------------------------- */
	/* Input vector													*/
	/* -------------------------------------------------------------------- */
	vector_float input_vector;
	input_vector.name = "Inputs vector";
	input_vector.length = 1;
	input_vector.orientation = 0;
	float data_input[1];
	input_vector.data = (float *) data_input;
	
	kalmanInit(&elevatorHandler);
			
	vector_float_set(&input_vector, 1, 3);
	vector_float_set(&measurement_vector, 1, 0.15);
	
	char temp[20];
	sprintf(temp, "Time: %dms %ds\r\n", milisecondsTimer, secondsTimer);
	usartBufferPutString(usart_buffer_4, temp, 10);
	
	int16_t i;
	for (i = 0; i < 140; i++) { //140
				
		kalmanIteration(&elevatorHandler, &measurement_vector, &input_vector, &A_matrix, &B_matrix, &R_matrix, &Q_matrix, &C_matrix, dt);
	}

	sprintf(temp, "Time: %dms %ds\r\n", milisecondsTimer, secondsTimer);
	usartBufferPutString(usart_buffer_4, temp, 10);
	
	vector_float_print(elevatorHandler.states);
	matrix_float_print(elevatorHandler.covariance);

	while (1) {
		
		altitudeEstimator();
		
		if (altitudeControllerEnabled == true)
			altitudeController();
		
		// makes the 70Hz loop
		vTaskDelay((int16_t) dt*((float) 1000));
	}
}