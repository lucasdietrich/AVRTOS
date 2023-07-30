pipeline {
    agent {
        docker { image 'fedora-avr-toolchain:latest' }
    }

    environment {
        CMAKE_GENERATOR='Ninja'

        WORKSPACE_BUILD_DIR = "build"
        WORKSPACE_OUTPUT_DIR = "output"
    }

    parameters {
        choice(
            name: 'CMAKE_BUILD_TYPE',
            choices: ['Release', 'Debug'],
            description: 'Build type',
        )
        choice(
            name: 'QEMU',
            choices: ['OFF', 'On'],
            description: 'Build for QEMU',
        )
        choice(
            name: 'CMAKE_TOOLCHAIN_FILE',
            choices: [
                'cmake/avr6-atmega2560.cmake', 
                'cmake/avr5-atmega328p.cmake',
                'cmake/avr5-atmega328pb.cmake',
                'cmake/avr5-board-caniot-tiny.cmake',
                'cmake/avr5-board-caniot-tiny-pb.cmake'
            ],
            description: 'Toolchain file to use',
        )
    }

    stages {
        stage('Versions') {
            steps {
                // Print versions of tools
                sh 'avr-gcc --version'
                sh 'cmake --version'
                sh 'ninja --version'
                sh 'python3 --version'
                sh 'qemu-system-avr --version'
            }
        }

        stage('Create python virtual environment') {
            steps {
                withEnv(["HOME=${env.WORKSPACE}"]) {
                    sh 'python3 -m venv .venv'
                    sh 'source .venv/bin/activate'
                    sh 'pip install --user -r scripts/requirements.txt'
                }
            }
        }

        stage('Configure (cmake)') {
            steps {
                sh """
                cmake -S . -B $WORKSPACE_BUILD_DIR \
                -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                -DCMAKE_TOOLCHAIN_FILE=${params.CMAKE_TOOLCHAIN_FILE} \
                -DCMAKE_GENERATOR=$CMAKE_GENERATOR \
                -DQEMU=${params.QEMU} \
                -DCMAKE_BUILD_TYPE=${params.CMAKE_BUILD_TYPE} \
                """
            }
        }

        stage('Build (ninja)') {
            steps {
                sh "ninja -C $WORKSPACE_BUILD_DIR"
            }
        }
        
        stage('Metrics') {
            steps {
                sh "scripts/metrics-collect.sh $WORKSPACE_BUILD_DIR $WORKSPACE_BUILD_DIR/metrics-exsizes.txt"
            }
        }

        stage('Export output') {
            steps {
                sh "mkdir -p $WORKSPACE_OUTPUT_DIR"
                sh "cp $WORKSPACE_BUILD_DIR/compile_commands.json $WORKSPACE_OUTPUT_DIR"
                sh "cp $WORKSPACE_BUILD_DIR/metrics-exsizes.txt $WORKSPACE_OUTPUT_DIR"
                dir("$WORKSPACE_BUILD_DIR") {
                    sh """
                        find examples -type f \\( \
                            -name '*.s' \
                            -o -name '*.txt' \
                            -o -name '*.out' \
                            -o -name '*.hex' \
                            -o -name '*.json' \
                            -o -name '*' -executable \
                        \\) -exec cp --parents {} ../$WORKSPACE_OUTPUT_DIR \\;
                    """
                }
            }
        }
    }
    post {
        always {
            dir("$WORKSPACE_BUILD_DIR") {
                deleteDir()
            }
        }
        success {
            archiveArtifacts artifacts: "$WORKSPACE_OUTPUT_DIR/**", fingerprint: true
        }
        // failure {
        // }
        // unstable {
        // }
        // changed {
        // }
    }
}
