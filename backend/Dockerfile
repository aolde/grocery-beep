FROM node:16

VOLUME [ "/root/.config/gkeep/auth.json" ]
WORKDIR /app

RUN apt-get update || : && apt-get install python3 python3-pip -y
RUN pip3 install -U gkeep==1.0.0 click==7.0

COPY index.js package.json /app/

RUN yarn --frozen-lockfile

ENTRYPOINT [ "node", "index.js" ]
