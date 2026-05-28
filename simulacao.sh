#!/bin/bash
echo "🚀 A iniciar o bombardeamento misto (Tráfego Normal vs Ataques)..."

# 1. Dispara 20 conexões de Tráfego Normal (HTTP)
for i in {1..20}; do
    echo "0,tcp,http,SF,232,8153,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,8,8,0.00,0.00,0.00,0.00,1.00,0.00,0.00,30,255,1.00,0.00,0.03,0.04,0.03,0.01,0.00,0.01" | ./client &
done

# 2. Dispara 20 conexões de Ataque (Neptune / DoS)
for i in {1..20}; do
    echo "0,tcp,private,S0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,123,6,1.00,1.00,0.00,0.00,0.05,0.07,0.00,255,26,0.10,0.05,0.00,0.00,1.00,1.00,0.00,0.00" | ./client &
done

# 3. Dispara 20 conexões de Tráfego Normal Diferente (FTP)
for i in {1..20}; do
    echo "0,tcp,ftp_data,SF,334,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2,2,0.00,0.00,0.00,0.00,1.00,0.00,0.00,2,39,1.00,0.00,1.00,0.10,0.00,0.00,0.00,0.00" | ./client &
done

# 4. Dispara 20 conexões de Ataque (Satan / Probe)
for i in {1..20}; do
    echo "0,tcp,private,REJ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,120,0.00,0.00,1.00,1.00,0.00,0.00,0.00,255,1,0.00,0.07,0.00,0.00,0.00,0.00,1.00,1.00" | ./client &
done

echo "Tornado de pacotes enviado! A aguardar as respostas do servidor..."
wait
echo "✅ Simulação concluída."
