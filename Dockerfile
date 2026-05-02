FROM nginx:alpine
COPY web-client /usr/share/nginx/html/
EXPOSE 80