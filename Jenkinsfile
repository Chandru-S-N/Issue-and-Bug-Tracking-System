pipeline {
    agent any

    stages {

        stage('Build') {
            steps {
                bat 'javac Main.java'
            }
        }

        stage('Test') {
            steps {
                bat 'java Main'
            }
        }

        stage('Deploy') {
            steps {
                echo 'Deployment Successful!'
            }
        }
    }
}
